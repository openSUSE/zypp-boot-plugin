#!/bin/bash

if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
fi

# [Packages] can also match via a key tagged with "provides:", like [main] does.
# "rpm" provides "rpminst", which is set to soft-reboot in [Packages].
./boot-plugin <../tests/packages_section_provides.protocol
grep soft-reboot $ZYPP_BOOT_PLUGIN_REBOOT_FILE
