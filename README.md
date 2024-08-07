# zypp-boot-plugin

This Zypp plugin will be called after each package installation/update.
If needed the plugin creates the file */run/reboot-needed* as an indicator
for other applications that a reboot is required.

There are different reboot levels which will be stored in the */run/reboot-needed* as an
ASCII string:

* **reboot** Hard reboot
* **kexec** Booting a new kernel without initializing hardware and without using any bootloader like Grub2.
* **soft-reboot** Soft reboot done by e.g. *systemctl soft-reboot*

## Evaluating the required Boot-Level

The plugin is getting a list of freshly installed/updated packages and
checks for each package if a reboot is required:

1. If the installed/updated package is contained in the package list defined in */usr/etc/zypp/zypp-boot-plugin.conf*:

   ```
   
   ## Configuration file for zypp-boot-plugin plugin.
   ## /usr/etc/zypp/zypp-boot-plugin.conf
   ##
   ## This configuration file defines packages for which a reboot
   ## is needed after they have been installed/updated.
   ##
   ## The package name can also be defined by regex expressions.
   ## This is needed for e.g. libraries with package names like
   ## libopenssl1_1, libopenssl3, ...
   ##
   
   [main]
   
   ##
   ## Packages for which a hard reboot is needed after they have been
   ## installed or updated.
   ##
   ## Packages are selected either by name, or by provides. In the later case
   ## the string must start with "provides:" immediately followed by the capability.
   ##
   ## Example:
   ##	foo				- just packages with name 'foo'
   ##	provides:bar                    - all packages providing 'bar'
   ##
   ## Valid values:
   ##	Comma separated list of packages.
   ##
   ## Default value:
   ##	empty
   ##
   reboot = grub2
   
   ##
   ## Packages for which a kexec call is needed at least after they have been
   ## installed or updated.
   ##
   ## Packages are selected either by name, or by provides. In the later case
   ## the string must start with "provides:" immediately followed by the capability.
   ##
   ## Example:
   ##	foo				- just packages with name 'foo'
   ##	provides:bar                    - all packages providing 'bar'
   ##
   ## Valid values:
   ##	Comma separated list of packages.
   ##
   ## Default value:
   ##	empty
   ##
   kexec = provides:multiversion(kernel)
   
   ##
   ## Packages for which a soft reboot is needed at least after they have been
   ## installed or updated.
   ##
   ## Packages are selected either by name, or by provides. In the later case
   ## the string must start with "provides:" immediately followed by the capability.
   ##
   ## Example:
   ##	foo				- just packages with name 'foo'
   ##	provides:bar                    - all packages providing 'bar'
   ##
   ## Valid values:
   ##	Comma separated list of packages.
   ##
   ## Default value:
   ##	empty
   ##
   soft-reboot = glibc, dbus-broker, dbus-1-daemon, libopenssl[0-9]?_?[0-9]?_?[0-9]?, libopenssl[0-9]?_?[0-9]?_?[0-9]?-32bit
   
   ```
   Admin/User changes should go into /etc/zypp/zypp-boot-plugin.conf for resetting all entries or should go
   into /etc/zypp/zypp-boot-plugin.conf.d/ for indiviual changes only. Have a look to :https://opensuse.github.io/libeconf/
   for more infmormation.
   These entries which will not be overwritten during a package update.

2. If a freshly installed/updated package is not defined in */usr/etc/zypp/zypp-boot-plugin.conf*, but its dependencies
   contain a *Provides* for *installhint(reboot-needed)*:

   If this dependency has been defined, a hard reboot will be done.

   Additionally the kind of reboot can also be set by e.g. *installhint(reboot-needed) = kexec* which is "weaker" than
   a hard reboot.

The result will be a list of packages together with the kind of required reboot-level. This list is the base of evaluating the correct entry
for */run/reboot-needed* where the "strongest" boot process in the list will be selected:

1. reboot
2. kexec
3. soft-reboot
