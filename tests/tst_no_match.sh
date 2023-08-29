#!/bin/bash

# Installed package cannot be found at all

./boot-plugin <../tests/no_match.protocol
if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
    return -1
fi
