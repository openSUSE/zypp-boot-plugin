# Building and installing libeconf

## Building

#### Prerequires:
* meson
* libeconf-devel
* libjson-c-devel

#### Build:
```
# meson <builddir>
# cd <builddir>
# meson compile
```

#### Installation:
```
# sudo meson install
```

## Releasing a new version

### Tagging new Release

1. Edit NEWS declaring the new version number and making all the changes to it.
2. Update the version number in meson.build.
2. Commit to git.
3. On https://github.com/openSUSE/zypp-boot-plugin click on releases on the right column (or go to https://github.com/openSUSE/zypp-boot-plugin/releases)
4. 'Draft a new release'.
5. In 'tag version' write vX.Y.Z matching the new version you declared in your NEWS commit.
6. Write a release title (e.g. "Release Version X.Y.Z")
7. In the description just copy/paste the NEWS entry.
8. Publish the release.

This creates a git tag for all the latest commits (hence the NEWS commit being important) under the new version number.
