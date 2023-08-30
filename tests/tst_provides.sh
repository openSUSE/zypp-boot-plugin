#!/bin/bash

if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
fi

# Installed package found by provides in configuration file
./boot-plugin <../tests/provides.protocol
grep kexec $ZYPP_BOOT_PLUGIN_REBOOT_FILE
