#ifndef ZYPP_PLUGIN_H
#define ZYPP_PLUGIN_H

#include <iostream>
#include "stomp.h"

class ZyppPlugin {
public:
    // Plugin message aka frame
    // https://doc.opensuse.org/projects/libzypp/HEAD/zypp-plugins.html
    using Message = Stomp::Message;

    /// Where the protocol reads from
    std::istream& pin;
    /// Where the protocol writes to
    std::ostream& pout;
    /// Where the plugin writes log messages to
    std::ostream& plog;
    ZyppPlugin(std::istream& in = std::cin,
	       std::ostream& out = std::cout,
	       std::ostream& log = std::cerr)
	: pin(in)
	, pout(out)
	, plog(log)
    {}

    virtual ~ZyppPlugin() = default;

    virtual int main();

protected:
    /// Handle a message and return a reply.
    // Derived classes should override it.
    // The base acks a _DISCONNECT and replies _ENOMETHOD to everything else.
    virtual Message dispatch(const Message&);

    Message read_message(std::istream& is) const { return Stomp::read_message(is); }
    void write_message(std::ostream& os, const Message& msg) const { Stomp::write_message(os, msg); }

    Message ack() const { return Stomp::ack(); }
};

#endif //ZYPP_PLUGIN_H
