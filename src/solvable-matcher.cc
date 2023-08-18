#include "solvable-matcher.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

Boot
SolvableMatcher::match_solvables(const set<string>& solvables, const std::string& cfg_filename)
{
    Boot ret = Boot::NONE;
    for (auto s: solvables) {
	cerr << "DEBUG:(boot-plugin): Searching boot flag for:" << s << endl;
    }

    return ret;
}

