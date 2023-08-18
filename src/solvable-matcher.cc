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

    for (auto s: solvables) {
	cerr << "DEBUG:(boot-plugin): Searching boot flag for:" << s << endl;
    }

    return ret;
}

