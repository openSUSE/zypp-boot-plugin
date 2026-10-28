#!/bin/bash

if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
fi

# zypp-boot-test-pkg is listed as "reboot" in [main] but overridden to "kexec"
# in [Packages], which has to win since [Packages] has a higher priority.
./boot-plugin <../tests/packages_section.protocol
grep kexec $ZYPP_BOOT_PLUGIN_REBOOT_FILE
