#include <iostream>
#include <boost/algorithm/string/trim.hpp>
#include <string>
#include <regex>

using namespace std;

#include "zypp-plugin.h"

int
ZyppPlugin::main()
{
    while(true)
    {
	Message msg = read_message(pin);
	if (pin.eof() && msg.command.length() <= 0)
	    break;
	Message reply = dispatch(msg);
	write_message(pout, reply);
        if (pin.eof())
	    break;
    }
    return 0;
}


ZyppPlugin::Message
ZyppPlugin::dispatch(const Message& msg)
{
    if (msg.command == "_DISCONNECT") {
	return ack();
    }
    Message a;
    a.command = "_ENOMETHOD";
    a.headers["Command"] = msg.command;
    return a;
}
