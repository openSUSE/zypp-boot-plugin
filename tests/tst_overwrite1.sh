#!/bin/bash

echo "reboot" > $ZYPP_BOOT_PLUGIN_REBOOT_FILE

# Installed package found by provides in configuration file
# Evaluated boot level is kexec but should not overwrite already existing
# reboot in ZYPP_BOOT_PLUGIN_REBOOT_FILE
./boot-plugin <../tests/provides.protocol
grep reboot $ZYPP_BOOT_PLUGIN_REBOOT_FILE
