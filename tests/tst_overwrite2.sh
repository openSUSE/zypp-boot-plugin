#!/bin/bash

echo "soft-reboot" > $ZYPP_BOOT_PLUGIN_REBOOT_FILE

# Installed package found by provides in configuration file
# Evaluated boot level is kexec and should overwrite already existing
# soft-reboot in ZYPP_BOOT_PLUGIN_REBOOT_FILE
./boot-plugin <../tests/provides.protocol
grep kexec $ZYPP_BOOT_PLUGIN_REBOOT_FILE
