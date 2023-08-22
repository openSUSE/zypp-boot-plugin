// split
#include <boost/algorithm/string/split.hpp> // boost::algorithm::split
#include <boost/algorithm/string/classification.hpp> // boost::is_any_of
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

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
        cerr << "DEBUG:(boot-plugin): Found package " << package_name << " for dependency " << dependency << endl;
	return true;
    }
    return false;
}

Boot
check_boot_level( const Boot current_boot_level, const Boot boot_level, const string package_name, econf_file *conffiles )
{
    Boot ret = current_boot_level;
    const char *boot_level_string = NULL;

    if (ret >= boot_level) return ret;

    switch(boot_level)
    {
    case Boot::HARD:
	boot_level_string = "reboot";
	break;
    case Boot::KEXEC:
	boot_level_string = "kexec";
	break;
    case Boot::SOFT:
	boot_level_string = "soft-reboot";
	break;
    default:
        cerr << "WARNING:(boot-plugin): given wrong boot level" << endl;
	return ret;
    }

    char *dep_list = NULL;
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
		std::cerr << "DEBUG:(boot-plugin): " << boot_level_string << ":" << flag << std::endl;

		// checking if the flag is the package name
		if (package_name == flag)
		{
		    ret = boot_level;
		} else {
  		    // checking if the package fullfill a given provides
 		    if (boost::starts_with(flag, "provides:")) {
		        flag = flag.substr(9);
			std::cerr << "DEBUG:(boot-plugin): checking provides " << flag << endl;
			if (provides(flag, package_name)) {
			   ret = boot_level;
			}
		    }
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

    cerr << "DEBUG:(boot-plugin): loading " << cfg_filename <<
	    POSTFIX << " from " <<
	    USR_CONF_DIR << " and " << CONF_DIR << endl;

    econf_file *conffiles;

    econf_err e_error = econf_readDirs(&conffiles, USR_CONF_DIR, CONF_DIR, cfg_filename.c_str(), "conf", "=", "#");
    if (e_error && e_error != ECONF_NOFILE) {
	econf_freeFile(conffiles);
	cerr << "ERROR:(boot-plugin): Cannot load these config files: " <<
		std::string(econf_errString(e_error)) << endl;
	return ret;
    }

    for (auto solvable: solvables) {
	boost::trim(solvable);
	cerr << "DEBUG:(boot-plugin): Searching boot flag for:" << solvable << endl;

        ret = check_boot_level(ret, Boot::HARD, solvable , conffiles);
        ret = check_boot_level(ret, Boot::KEXEC, solvable , conffiles);
	ret = check_boot_level(ret, Boot::SOFT, solvable , conffiles);
    }

    return ret;
}

