#!/bin/bash

if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
fi

# Installed kernel package with type M found by provides in configuration file
./boot-plugin <../tests/kernel.protocol
grep kexec $ZYPP_BOOT_PLUGIN_REBOOT_FILE
