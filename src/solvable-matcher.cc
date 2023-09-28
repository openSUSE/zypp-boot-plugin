// split
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp> // boost::is_any_of
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <string>
#include <libeconf.h>
#include <vector>
#include <cstdlib>
#include "solvable-matcher.h"

using namespace std;

#define POSTFIX ".conf"

bool
provides( const string dependency, const string package_name)
{
    string command = "/bin/rpm -q --whatprovides --qf '%{NAME}\\n' '" +
	    dependency + "'|/usr/bin/grep -Pzq '" + package_name + "\\n'";
    if (system(command.c_str()) == 0)
    {
        cerr << "DEBUG:(boot-plugin):  --- Found package " << package_name << " for dependency " << dependency << endl;
	return true;
    }
    return false;
}

bool
check_installhint( const string boot_level_string, const string package_name)
{
    if (provides("installhint(reboot-needed)", package_name)) {
	// checking boot level if provided
	string command = "/bin/rpm -q --provides " + package_name +
		"|/usr/bin/grep -Pzq 'installhint\\(reboot-needed\\) = " + boot_level_string + "\\n'";
	if (system(command.c_str()) == 0)
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
		boost::regex expression(flag);
		boost::cmatch what;
		if (boost::regex_match(package_name.c_str(), what, expression)) {
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

