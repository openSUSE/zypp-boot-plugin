# zypp-boot-plugin

This Zypp plugin will be called after each package installation/update with will be done
by applications which uses libzypp.
If needed the plugin creates the file */run/reboot-needed* which indicates
for other applications, that a reboot is needed.

There are different boot levels which will be stored in the */run/reboot-needed* as an normal
ASCII string:

* **reboot** Hard reboot
* **kexec** Booting a new kernel without initializing hardware and without using any bootloader like Grub2.
* **soft-reboot** Soft reboot done by E.G. *systemctl soft-reboot*

## Evaluating the needed Boot-Level

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

   ```

2.If an fresh installed/updated package is not defined in */etc/zypp/zypp-boot-plugin.conf* the provides dependency
  of this package will be checked for the tag *installhint(reboot-needed)*.
  If this dependency has been defined, a hard reboot will be done.

  Additional, the kind of reboot can also be set by E.G. *installhint(reboot-needed) = kexec* which is "weaker" than
  a hard reboot.


The result will be a list of packages together with the kind of needed boot-level. This list is the base of evaluating the correct entry
in */run/reboot-needed* where the "strongest" boot process in the list will be taken:

1. reboot
2. kexec
3. soft-boot



## Building plugin

#### Prerequires:
* meson
* libeconf

#### Build:
```
# meson <builddir>
# cd <builddir>
# meson compile
```

#### Installation:
```
# meson install
```

