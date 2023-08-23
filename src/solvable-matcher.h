#ifndef SOLVABLE_MATCHER_H
#define SOLVABLE_MATCHER_H

#include <iostream>
#include <string>
#include <set>

#define SOFTSTR "soft-reboot"
#define KEXECSTR "kexec"
#define HARDSTR "reboot"

enum class Boot { NONE, SOFT, KEXEC, HARD };

struct SolvableMatcher {
    static Boot match_solvables(const std::set<std::string>& solvables, const std::string& cfg_filename);
};

#endif //SOLVABLE_MATCHER_H
