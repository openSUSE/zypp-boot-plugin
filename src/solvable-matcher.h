#ifndef SOLVABLE_MATCHER_H
#define SOLVABLE_MATCHER_H

#include <iostream>
#include <string>
#include <set>

enum class Boot { NONE, SOFT, KEXEC, HARD };

struct SolvableMatcher {
    static Boot match_solvables(const std::set<std::string>& solvables, const std::string& cfg_filename);
};

#endif //SOLVABLE_MATCHER_H
