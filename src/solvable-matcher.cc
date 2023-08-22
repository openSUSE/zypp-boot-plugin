// split
#include <boost/algorithm/string/split.hpp> // boost::algorithm::split
#include <boost/algorithm/string/classification.hpp> // boost::is_any_of
#include <boost/algorithm/string/trim.hpp>

#include <string>
#include <libeconf.h>
#include <vector>
#include "solvable-matcher.h"

using namespace std;

#define POSTFIX ".conf"

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

	//soft-reboot
        char *soft_reboot_string = NULL;
        e_error = econf_getStringValue(conffiles, "main", "soft-reboot", &soft_reboot_string);
        if (e_error)
        {
	    cerr << "Warning:(boot-plugin): Cannot read 'soft-reboot': " <<
		    std::string(econf_errString(e_error)) <<
		    endl;
            continue;
        } else {
            if (soft_reboot_string != NULL) {
   	        std::string soft_reboot_list_string(soft_reboot_string);
	        // Split by ","
	        std::vector<std::string> splitResult;
	        boost::algorithm::split(splitResult, soft_reboot_list_string, boost::is_any_of(","));

	        for(std::string& flag : splitResult) {
		    boost::trim(flag);
	            std::cerr << "DEBUG:(boot-plugin): soft-reboot flag:" << flag << std::endl;
		    if ((solvable == flag) && (ret < Boot::SOFT))
			 ret = Boot::SOFT;
	        }
	    }
	}

	// kexec
        char *kexec_string = NULL;
        e_error = econf_getStringValue(conffiles, "main", "kexec", &kexec_string);
        if (e_error)
        {
	    cerr << "Warning:(boot-plugin): Cannot read 'kexec': " <<
		    std::string(econf_errString(e_error)) <<
		    endl;
            continue;
        } else {
            if (kexec_string != NULL) {
   	        std::string kexec_list_string(kexec_string);
	        // Split by ","
	        std::vector<std::string> splitResult;
	        boost::algorithm::split(splitResult, kexec_list_string, boost::is_any_of(","));

	        for(std::string& flag : splitResult) {
		    boost::trim(flag);
	            std::cerr << "DEBUG:(boot-plugin): kexec flag:" << flag << std::endl;
		    if ((solvable == flag) && (ret < Boot::KEXEC))
			 ret = Boot::KEXEC;
	        }
	    }
	}

	//hard reboot
        char *hard_reboot_string = NULL;
        e_error = econf_getStringValue(conffiles, "main", "reboot", &hard_reboot_string);
        if (e_error)
        {
	    cerr << "Warning:(boot-plugin): Cannot read 'reboot': " <<
		    std::string(econf_errString(e_error)) <<
		    endl;
            continue;
        } else {
            if (hard_reboot_string != NULL) {
   	        std::string hard_reboot_list_string(hard_reboot_string);
	        // Split by ","
	        std::vector<std::string> splitResult;
	        boost::algorithm::split(splitResult, hard_reboot_list_string, boost::is_any_of(","));

	        for(std::string& flag : splitResult) {
		    boost::trim(flag);
		    std::cerr << "DEBUG:(boot-plugin): reboot flag:" << flag << std::endl;
		    if ((solvable == flag) && (ret < Boot::HARD))
			ret = Boot::HARD;
	        }
	    }
	}
    }

    return ret;
}

