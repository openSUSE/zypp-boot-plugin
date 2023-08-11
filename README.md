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

