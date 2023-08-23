project('zypp-boot_plugin', 'cpp',
	version : '0.0.1',
	license : 'LGPLv3',
	default_options : [
		'buildtype=debugoptimized',
		'default_library=shared',
		'b_pie=true',
		'warning_level=2',
		'prefix=/usr',
		'sysconfdir=/etc',
	],
	meson_version : '>= 0.49.0',
)

cxx = meson.get_compiler('cpp')

boot_plugin_src = files(
  'src/zypp-boot-plugin.cc',
  'src/solvable-matcher.cc',
  'src/zypp-commit-plugin.cc',
  'src/zypp-plugin.cc',
)

libeconf_dep = dependency('libeconf')
boost_dep = dependency('boost')
json_dep = dependency('json')

pk_prefix = get_option('prefix')
pk_sysconfdir = get_option('sysconfdir')

c_flags = [
  '-DUSR_CONF_DIR="@0@@1@/zypp"'.format(pk_prefix, pk_sysconfdir),
  '-DCONF_DIR="@0@/zypp"'.format(pk_sysconfdir)
]

boot_plugin = executable(
  'boot-plugin',
  boot_plugin_src,
  dependencies : [boost_dep, libeconf_dep, json_dep],
  cpp_args: c_flags,
  install : true,
  install_dir : '/usr/lib/zypp/plugins/commit'
)

config = files(
  'conf/zypp-boot-plugin.conf',
)

install_data(
  config,
  install_dir : '@0@@1@/zypp'.format(pk_prefix, pk_sysconfdir)
)

# Manual pages
subdir('man')

# Unit tests
#subdir('testsuite')
