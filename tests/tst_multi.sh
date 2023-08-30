#!/bin/bash

if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
fi

# Packages have been installed and the "strongest" has won.
./boot-plugin <../tests/multi.protocol
grep reboot $ZYPP_BOOT_PLUGIN_REBOOT_FILE
