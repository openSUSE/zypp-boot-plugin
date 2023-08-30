#!/bin/bash

if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
fi

# Installed package found by name in configuration file
./boot-plugin <../tests/package_name.protocol
grep soft-reboot $ZYPP_BOOT_PLUGIN_REBOOT_FILE
