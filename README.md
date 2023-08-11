# zypp-boot-plugin

This Zypp plugin will be called after each package installation/update with will be done
by applications which uses libzypp.
If needed the plugin creates the file */run/reboot-needed* which indicates
for other applications, that a reboot is needed.

There are diffenent boot levels which will be stored in the */run/reboot-needed* as an normal
ASCII string:

* **reboot** Hard reboot
* **kexec** Booting a new kernel without initializing hardware and without using any bootloader like Grub2.
* **soft-reboot** Soft reboot done by E.G. *systemctl soft-reboot*

## Evaluting the needed Bootlevel

The plugin is getting a list of fresh installed/updated packages. The plugin
checks for each package if a reboot is needed:

1. The plugin checks if the installed/updated package is in the package list defined in */etc/zypp/zypp-boot-plugin.conf*:

   ```
   ## Configuration file for zypp-boot-plugin plugin.
   ## /etc/zypp/zypp-boot-plugin.conf
   ##
   ## This configuration file defines packages for which a reboot
   ## is needed after they have been installed/updated.
   
   [main]
   
   ##
   ## Packages for which a hard reboot is needed after they have been
   ## installed or updated.
   ##
   ## Packages are selected either by name, or by provides. In the later case
   ## the string must start with "provides:" immediately followed by the capability.
   ##
   ## Example:
   ##	foo				- just packages whith name 'foo'
   ##	provides:bar                    - all packages providing 'bar'
   ##
   ## Valid values:
   ##	Comma separated list of packages.
   ##
   ## Default value:
   ##	empty
   ##
   reboot = grub2, glibc
   
   ##
   ## Packages for which a kexec call is needed at least after they have been
   ## installed or updated.
   ##
   ## Packages are selected either by name, or by provides. In the later case
   ## the string must start with "provides:" immediately followed by the capability.
   ##
   ## Example:
   ##	foo				- just packages whith name 'foo'
   ##	provides:bar                    - all packages providing 'bar'
   ##
   ## Valid values:
   ##	Comma separated list of packages.
   ##
   ## Default value:
   ##	empty
   ##
   kexec = multiversion(kernel)
   
   ##
   ## Packages for which a soft reboot is needed at least after they have been
   ## installed or updated.
   ##
   ## Packages are selected either by name, or by provides. In the later case
   ## the string must start with "provides:" immediately followed by the capability.
   ##
   ## Example:
   ##	foo				- just packages whith name 'foo'
   ##	provides:bar                    - all packages providing 'bar'
   ##
   ## Valid values:
   ##	Comma separated list of packages.
   ##
   ## Default value:
   ##	empty
   ##
   soft-reboot = libopenssl
   
   ##
   ## Packages for which a userland restart is needed at least after they have been
   ## installed or updated and are running in an userland environment.
   ##
   ## Packages are selected either by name, or by provides. In the later case
   ## the string must start with "provides:" immediately followed by the capability.
   ##
   ## Example:
   ##	foo				- just packages whith name 'foo'
   ##	provides:bar                    - all packages providing 'bar'
   ##
   ## Valid values:
   ##	Comma separated list of packages.
   ##
   ## Default value:
   ##	empty
   ##
   userland = vi

   ```

2.