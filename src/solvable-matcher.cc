#include "solvable-matcher.h"
#include <string>
#include <libeconf.h>

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

    econf_err error = econf_readDirs(&conffiles, USR_CONF_DIR, CONF_DIR, cfg_filename.c_str(), "conf", "=", "#");
    if (error && error != ECONF_NOFILE) {
	econf_freeFile(conffiles);
	cerr << "ERROR:(boot-plugin): Cannot load these config files: " <<
		std::string(econf_errString(error)) << endl;
	return Boot::NONE;
    }

    for (auto s: solvables) {
	cerr << "DEBUG:(boot-plugin): Searching boot flag for:" << s << endl;
    }

    return ret;
}

