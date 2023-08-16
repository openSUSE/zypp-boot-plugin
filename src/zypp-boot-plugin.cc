// getenv
#include <stdlib.h>

// split
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <json.h>

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

    ProgramOptions()
    {
	const char* s;

	s = getenv("ZYPP_BOOT_PLUGIN_CONFIG");
	if (s)
	    plugin_config = s;
	else
  	    plugin_config = "zypp-boot-plugin.conf";
    }
};


class ZyppBootPlugin : public ZyppCommitPlugin
{
public:
    ZyppBootPlugin(const ProgramOptions& opts)
    : solvable_matchers(SolvableMatcher::load_config(opts.plugin_config))
    {
    }

    Message plugin_begin(const Message& m) override {
	cerr << "INFO:" << "PLUGINBEGIN" << endl;
	userdata = get_userdata(m);

	return ack();
    }
    Message plugin_end(const Message& m) override {
	UNUSED(m);
	cerr << "INFO:" << "PLUGINEND" << endl;
	return ack();
    }
    Message commit_begin(const Message& msg) override {
	cerr << "INFO:" << "COMMITBEGIN" << endl;

	set<string> solvables = get_solvables(msg, Phase::BEFORE);
	cerr << "DEBUG:" << "solvables: " << solvables << endl;

	bool found, important;
	match_solvables(solvables, found, important);
	cerr << "INFO:" << "found: " << found << ", important: " << important << endl;

	if (found || important) {
	    userdata["important"] = important ? "yes" : "no";

	    cerr << "INFO:" << "creating pre snapshot" << endl;
	    // creating here FIXME
	    cerr << "DEBUG:" << "created pre snapshot " << endl;
	}

	return ack();
    }

    Message commit_end(const Message& msg) override {
	cerr << "INFO:" << "COMMITEND" << endl;

        set<string> solvables = get_solvables(msg, Phase::AFTER);
        cerr << "DEBUG:" << "solvables: " << solvables << endl;

        bool found, important;
        match_solvables(solvables, found, important);
        cerr << "INFO:" << "found: " << found << ", important: " << important << endl;

        if (found || important) {
   	   userdata["important"] = important ? "yes" : "no";

  	   cerr << "INFO:" << "setting snapshot data" << endl;
 	   cerr << "INFO:" << "creating post snapshot" << endl;
 	   // FIXME
	}
	else {
 	   cerr << "INFO:" << "deleting pre snapshot" << endl;
	   // FIXME
	}
	return ack();
    }

private:
    static const string cleanup_algorithm;

    map<string, string> userdata;

    vector<SolvableMatcher> solvable_matchers;

    map<string, string> get_userdata(const Message&);

    enum class Phase { BEFORE, AFTER };

    set<string> get_solvables(const Message&, Phase phase);

    void match_solvables(const set<string>& solvables, bool& found, bool& important);

    unsigned int create_pre_snapshot(string config_name, string description, string cleanup, map<string, string> userdata);
};


const string ZyppBootPlugin::cleanup_algorithm = "number";


map<string, string>
ZyppBootPlugin::get_userdata(const Message& msg)
{
    map<string, string> result;
    auto it = msg.headers.find("userdata");
    if (it != msg.headers.end()) {
	const string& userdata_s = it->second;
	vector<string> key_values;
	boost::split(key_values, userdata_s, boost::is_any_of(","));
	for (auto kv : key_values)
	{
	    static const regex rx_keyval("([^=]*)=(.+)", regex::extended);
	    smatch match;

	    if (regex_match(kv, match, rx_keyval))
	    {
		string key = boost::trim_copy(match[1].str());
		string value = boost::trim_copy(match[2].str());
		result[key] = value;
	    }
	    else
	    {
		cerr << "ERROR:" << "invalid userdata: expecting comma separated key=value pairs" << endl;
	    }
	}
    }
    return result;
}

static
json_object*
object_get(json_object* obj, const char* name)
{
    json_object * result;
    if (!json_object_object_get_ex(obj, name, &result)) {
	cerr << "ERROR:" << '"' << name << "\" not found" << endl;
	return NULL;
    }
    return result;
}

set<string>
ZyppBootPlugin::get_solvables(const Message& msg, Phase phase)
{
    set<string> result;

    json_tokener * tok = json_tokener_new();
    json_object * zypp = json_tokener_parse_ex(tok, msg.body.c_str(), msg.body.size());
    json_tokener_error jerr = json_tokener_get_error(tok);
    if (jerr != json_tokener_success) {
	cerr << "ERROR:" << "parsing zypp JSON failed: "
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
	    bool have_type = json_object_object_get_ex(step, "type", NULL);
	    bool have_stage = json_object_object_get_ex(step, "stage", NULL);
	    if (have_type && (phase == Phase::BEFORE || have_stage)) {
		json_object * solvable = object_get(step, "solvable");
		if (!solvable) {
		    cerr << "ERROR:" << "in item #" << i << endl;
		    continue;
		}
		json_object * name = object_get(solvable, "n");
		if (!name) {
		    cerr << "ERROR:" << "in item #" << i << endl;
		    continue;
		}
		if (json_object_get_type(name) != json_type_string) {
		    cerr << "ERROR:" << "\"n\" is not a string" << endl;
		    cerr << "ERROR:" << "in item #" << i << endl;
		    continue;
		}
		else {
		    const char * prize = json_object_get_string(name);
		    result.insert(prize);
		}
	    }
	}
    }

    return result;
}


void
ZyppBootPlugin::match_solvables(const set<string>& solvables, bool& found, bool& important)
{
    found = false;
    important = false;
    for (auto s: solvables) {
	for (auto matcher: solvable_matchers) {
	    if (matcher.match(s)) {
		found = true;
		important = important || matcher.important;
		if (found && important)
		    return; // short circuit
	    }
	}
    }
}


int
main()
{
    if (getenv("DISABLE_ZYPP_BOOT_PLUGIN") != nullptr) {
	cerr << "INFO:" << "$DISABLE_ZYPP_BOOT_PLUGIN is set - disabling boot-plugin" << endl;
	ZyppCommitPlugin plugin;
	return plugin.main();
    }

    ProgramOptions options;
    ZyppBootPlugin plugin(options);
    return plugin.main();
}
