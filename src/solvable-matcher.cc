// split
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp> // boost::is_any_of
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/version.hpp>
#if BOOST_VERSION < 108800
#include <boost/process/child.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/io.hpp>
#else
#define BOOST_PROCESS_VERSION 1
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/search_path.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/pipe.hpp>
#endif

#include <string>
#include <libeconf.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <regex>
#include "solvable-matcher.h"

using namespace std;
namespace bp = boost::process;

#define POSTFIX ".conf"

bool
provides( const string dependency, const string package_name)
{
    static std::map<std::string, std::vector<std::string>> stored_provides;
    std::map<std::string, std::vector<std::string>>::iterator it;
    bool found = false;

    it =  stored_provides.find(dependency);
    if (it == stored_provides.end()) {
        bp::pipe p;
	bp::ipstream is;
	bp::child rpm(bp::search_path("rpm"), "-q", "--whatprovides", "--qf", "%{NAME}\\n", dependency, bp::std_out > is);
	std::string line;

	std::vector<std::string> packages;
	while (rpm.running() && std::getline(is, line)) {
            boost::trim(line);
	    if (boost::equals(line, package_name)) {
		found = true;
	    }
	    packages.push_back(line);
	}
	stored_provides[dependency] = packages;
	rpm.wait();
    } else {
	std::vector<std::string>::const_iterator i;
	for (i=((it->second).begin()); i!=((it->second).end()); ++i){
	    if (boost::equals(*i, package_name)) {
	        found = true;
	    }
	}
    }

    if (found)
	cerr << "DEBUG:(boot-plugin):  --- Found package " << package_name << " for dependency " << dependency << endl;

    return found;
}

bool
check_installhint( const string boot_level_string, const string package_name)
{
    if (provides("installhint(reboot-needed)", package_name)) {
	// checking boot level if provided
        string search_str = "installhint(reboot-needed) = " + boot_level_string;
	bp::ipstream is;
	bool found = false;
	bp::child rpm(bp::search_path("rpm"), "-q", "--provides", package_name, bp::std_out > is);
	
	std::string line;
	while (rpm.running() && std::getline(is, line)) {
            boost::trim(line);
	    if (boost::equals(line, search_str))
		found = true;
	}
	rpm.wait();

	if (found)
	{
	    cerr << "DEBUG:(boot-plugin):  --- Found boot dependency 'installhint(reboot-needed) = " << boot_level_string <<
		    "' for package " << package_name << endl;
	    return true;
	} else {
            if (boot_level_string == "reboot") {
	        // boot level is "reboot" (hard reboot) and installhint(reboot-needed) is provided
	        // So it has to be a hard reboot
	        cerr << "DEBUG:(boot-plugin):  --- set hard reboot for package " << package_name << endl;
 	        return true;
	    }
	}
    }
    return false;
}

Boot
check_boot_level( const Boot current_boot_level, const Boot boot_level, const string package_name, econf_file *conffiles )
{
    /* The function will be called for the same package for boot level "soft", "kexec" and "hard" in that order. */
    /* If one install hint matches no one else has to be checked for other boot levels and for that package.     */
    static bool installhint_found = false;

    Boot ret = current_boot_level;
    const char *boot_level_string = NULL;

    if (ret >= boot_level) return ret;

    switch(boot_level)
    {
    case Boot::HARD:
	    boot_level_string = HARDSTR;
	break;
    case Boot::KEXEC:
	    boot_level_string = KEXECSTR;
	break;
    case Boot::SOFT:
	    boot_level_string = SOFTSTR;
	    /* A new package will be checked. So the install hints has be checked again. */
	    installhint_found = false;
	break;
    default:
        cerr << "WARNING:(boot-plugin): given wrong boot level" << endl;
	return ret;
    }

    char *dep_list = NULL;
    bool found = false;
    econf_err e_error = econf_getStringValue(conffiles, "main", boot_level_string, &dep_list);
    if (e_error)
    {
        cerr << "Warning:(boot-plugin): Cannot read '" << boot_level_string << "': " <<
		std::string(econf_errString(e_error)) <<
		endl;
	return ret;
    } else {
        if (dep_list != NULL) {
   	    std::string dep_list_string(dep_list);
	    // Split by ","
	    std::vector<std::string> splitResult;
	    boost::algorithm::split(splitResult, dep_list_string, boost::is_any_of(","));

	    for(std::string& flag : splitResult) {
		boost::trim(flag);
		// cerr << "DEBUG:(boot-plugin): - checking config entry " << boot_level_string << ": " << flag << endl;

		// checking if the flag is the package name
		regex expression(flag);
		smatch what;
		if (regex_match(package_name, what, expression)) {
		    ret = boot_level;
		    found = true;
                    cerr << "DEBUG:(boot-plugin):  --- Found package " << package_name << endl;
		    cerr << "DEBUG:(boot-plugin):      Found for level: " << boot_level_string << endl;
		} else if (boost::starts_with(flag, "provides:")) {
  		    // checking if the package fullfill a given provides
		    flag = flag.substr(9);
		    // cerr << "DEBUG:(boot-plugin):  - checking provides " << flag << endl;
		    if (provides(flag, package_name)) {
			ret = boot_level;
			found = true;
			cerr << "DEBUG:(boot-plugin):      Found for level: " << boot_level_string << endl;
		    }
		}
	    }
	    if (!found && !installhint_found) {
		// cerr << "DEBUG:(boot-plugin): - checking for provides installhint(reboot-needed) (evtl. boot level)" << endl;
		if (check_installhint( boot_level_string, package_name)) {
		    installhint_found = true;
		    cerr << "DEBUG:(boot-plugin):      Found for level: " << boot_level_string << endl;
		    ret = boot_level;
		}
	    }   
	}
    }

    return ret;
}

Boot
SolvableMatcher::match_solvables(const set<string>& solvables, const std::string& cfg_filename)
{
    Boot ret = Boot::NONE;

    econf_file *conffiles;

    // Reading one file only at first
    econf_err e_error = econf_readFile(&conffiles, cfg_filename.c_str(), "=", "#");
    if (e_error == ECONF_NOFILE) {
        cerr << "DEBUG:(boot-plugin): loading " << cfg_filename <<
		POSTFIX << " from " <<
		USR_CONF_DIR << " and " << CONF_DIR << endl;
	e_error = econf_readDirs(&conffiles, USR_CONF_DIR, CONF_DIR, cfg_filename.c_str(), "conf", "=", "#");
    } else {
       cerr << "DEBUG:(boot-plugin): loaded " << cfg_filename << endl;
    }
    if (e_error) {
	cerr << "ERROR:(boot-plugin): Cannot load these config files: " <<
		std::string(econf_errString(e_error)) << endl;
	return ret;
    }

    for (auto solvable: solvables) {
	boost::trim(solvable);
	//cerr << "DEBUG:(boot-plugin): searching boot flag for: " << solvable << endl;

	ret = check_boot_level(ret, Boot::SOFT, solvable , conffiles);
        ret = check_boot_level(ret, Boot::KEXEC, solvable , conffiles);
        ret = check_boot_level(ret, Boot::HARD, solvable , conffiles);
    }
    econf_freeFile(conffiles);    

    return ret;
}

