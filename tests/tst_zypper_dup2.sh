#!/bin/bash

if test -f "$ZYPP_BOOT_PLUGIN_REBOOT_FILE"; then
    rm $ZYPP_BOOT_PLUGIN_REBOOT_FILE
fi

# kernel initiates kexec
./boot-plugin <../tests/zypper_dup2.protocol
grep kexec $ZYPP_BOOT_PLUGIN_REBOOT_FILE
