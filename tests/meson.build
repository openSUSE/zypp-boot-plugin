# This file runs the unit tests

test('tst-no-match', find_program(meson.current_source_dir() +'/tst_no_match.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_no_match.out'])

test('tst-package-name', find_program(meson.current_source_dir() +'/tst_package_name.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_package_name.out'])

test('tst-provides', find_program(meson.current_source_dir() +'/tst_provides.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_provides.out'])

test('tst-multi', find_program(meson.current_source_dir() +'/tst_multi.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_multi.out'])

test('tst-zypper_dup', find_program(meson.current_source_dir() +'/tst_zypper_dup.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_zypper_dup.out'])

test('tst-zypper_dup2', find_program(meson.current_source_dir() +'/tst_zypper_dup2.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_zypper_dup2.out'])

test('tst-overwrite1', find_program(meson.current_source_dir() +'/tst_overwrite1.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_overwrite1.out'])

test('tst-overwrite2', find_program(meson.current_source_dir() +'/tst_overwrite2.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_overwrite2.out'])

test('tst-packages-section', find_program(meson.current_source_dir() +'/tst_packages_section.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_packages_section.out'])

test('tst-packages-section-provides', find_program(meson.current_source_dir() +'/tst_packages_section_provides.sh'),
  env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_packages_section_provides.out'])

if host_machine.cpu() == 'x86_64'
   test('tst-kernel', find_program(meson.current_source_dir() +'/tst_kernel.sh'),
     env : ['ZYPP_BOOT_PLUGIN_CONFIG=../tests/zypp-boot-plugin.conf', 'ZYPP_BOOT_PLUGIN_REBOOT_FILE=./tst_kernel.out'])
endif
