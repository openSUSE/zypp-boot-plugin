// getenv
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <json.h>
#include <string.h>

using namespace std;

#include "zypp-commit-plugin.h"
#include "solvable-matcher.h"

ostream&
operator<<(ostream& os, const set<string>& ss)
{
    bool seen_first = false;
    os << '{';
    for (auto s : ss)
    {
	if (seen_first)
	    os << ", ";

	os << s;
	seen_first = true;
    }
    os << '}';
    return os;
}


class ProgramOptions
{
public:

    string plugin_config;
    string reboot_file;

    ProgramOptions()
    {
	const char* s;

	s = getenv("ZYPP_BOOT_PLUGIN_CONFIG");
	if (s)
	    plugin_config = s;
	else
  	    plugin_config = "zypp-boot-plugin";

	s = getenv("ZYPP_BOOT_PLUGIN_REBOOT_FILE");
	if (s)
	    reboot_file = s;
	else
	    reboot_file = "/run/reboot-needed";
    }
};


class ZyppBootPlugin : public ZyppCommitPlugin
{
public:
    Message plugin_begin(const Message& m) override {
	cerr << "INFO:(boot-plugin):" << m.command << endl;
	return ack();
    }
    Message plugin_end(const Message& m) override {
	cerr << "INFO:(boot-plugin):" << m.command << endl;
	return ack();
    }
    Message commit_begin(const Message& m) override {
	cerr << "INFO:(boot-plugin):" << m.command << endl;
	return ack();
    }

    Message commit_end(const Message& msg) override {
        cerr << "INFO:(boot-plugin):" << msg.command << " BEGIN" << endl;

        set<string> solvables = get_solvables(msg);
        // cerr << "DEBUG:(boot-plugin):" << "solvables: " << solvables << endl;

	ProgramOptions opts;
        Boot bootkind = SolvableMatcher::match_solvables(solvables, opts.plugin_config);

        if (bootkind != Boot::NONE) {
           cerr << "INFO:(boot-plugin):" << "Set '" << boot_to_str(bootkind) <<
		   "' in " << opts.reboot_file << endl;
	   ofstream outputfile;
           outputfile.open (opts.reboot_file);
           outputfile << boot_to_str(bootkind);
           outputfile.close();
	}

	cerr << "INFO:(boot-plugin):" << msg.command << " END" << endl;

	return ack();
    }

private:

    set<string> get_solvables(const Message&);
    const char* boot_to_str(Boot boot);
};


static
json_object*
object_get(json_object* obj, const char* name)
{
    json_object * result;
    if (!json_object_object_get_ex(obj, name, &result)) {
	cerr << "ERROR:(boot-plugin):" << '"' << name << "\" not found" << endl;
	return NULL;
    }
    return result;
}

const char*
ZyppBootPlugin::boot_to_str(Boot boot)
{
    switch(boot)
    {
    case Boot::HARD:
	return HARDSTR;
    case Boot::KEXEC:
	return KEXECSTR;
    case Boot::SOFT:
        return SOFTSTR;
    default:
        return "NONE";
    }
}

set<string>
ZyppBootPlugin::get_solvables(const Message& msg)
{
    cerr << "DEBUG:(boot-plugin):msg.body:" << msg.body << endl;
    set<string> result;

    json_tokener * tok = json_tokener_new();
    json_object * zypp = json_tokener_parse_ex(tok, msg.body.c_str(), msg.body.size());
    json_tokener_error jerr = json_tokener_get_error(tok);
    if (jerr != json_tokener_success) {
	cerr << "ERROR:(boot-plugin):" << "parsing zypp JSON failed: "
			 << json_tokener_error_desc(jerr) << endl;
	return result;
    }

    // JSON structure:
    // {"TransactionStepList":[{"type":"?","stage":"?","solvable":{"n":"mypackage"}}]}
    // https://doc.opensuse.org/projects/libzypp/HEAD/plugin-commit.html
    json_object * steps = object_get(zypp, "TransactionStepList");
    if (!steps)
	return result;

    if (json_object_get_type(steps) == json_type_array) {
	size_t i, len = json_object_array_length(steps);
	for (i = 0; i < len; ++i) {
	    json_object * step = json_object_array_get_idx(steps, i);

	    json_object * type = object_get(step, "type");
	    if (!type) {
	        cerr << "ERROR:(boot-plugin):" << "no -type- in item #" << i << endl;
	        continue;
	    }
	    const char * type_c = json_object_get_string(type);
	    if (strcmp(type_c, "+") != 0) {
	        cerr << "DEBUG:(boot-plugin):" << "ignoring type: " << type_c <<" in item #" << i << endl;
	        continue;
	    }

	    json_object * stage = object_get(step, "stage");
	    if (!stage) {
	        cerr << "ERROR:(boot-plugin):" << "no -stage- in item #" << i << endl;
	        continue;
	    }
	    const char * stage_c = json_object_get_string(stage);
	    if (strcmp(stage_c, "ok") != 0) {
	        cerr << "DEBUG:(boot-plugin):" << "ignoring stage: " << stage_c <<" in item #" << i << endl;
	        continue;    
	    }

	    json_object * solvable = object_get(step, "solvable");
	    if (!solvable) {
	        cerr << "ERROR:(boot-plugin):" << "in item #" << i << endl;
	        continue;
	    }
	    json_object * name = object_get(solvable, "n");
	    if (!name) {
	        cerr << "ERROR:(boot-plugin):" << "in item #" << i << endl;
	        continue;
	    }
	    if (json_object_get_type(name) != json_type_string) {
	        cerr << "ERROR:(boot-plugin):" << "\"n\" is not a string" << endl;
	        cerr << "ERROR:(boot-plugin):" << "in item #" << i << endl;
	        continue;
	    }
	    else {
	        const char * prize = json_object_get_string(name);
	        result.insert(prize);
	    }
	}
    }

    return result;
}

int
main()
{
    if (getenv("DISABLE_ZYPP_BOOT_PLUGIN") != nullptr) {
	cerr << "INFO:(boot-plugin):" << "$DISABLE_ZYPP_BOOT_PLUGIN is set - disabling boot-plugin" << endl;
	ZyppCommitPlugin plugin;
	return plugin.main();
    }

    ZyppBootPlugin plugin;
    return plugin.main();
}
