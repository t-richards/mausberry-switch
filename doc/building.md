# Building & packaging from source

This guide covers compiliation and package assembly for Debian 7 "Wheezy", and
Debian 8 "Jessie" only. Other distributions are not officially supported.

# Requirements

 - [Autotools][autotools]
 - C compiler
 - [GLib][glib] development/runtime libraries

# Building from source, no packaging

```bash
# Install compiler and dependencies
$ sudo apt-get install build-essential dh-autoreconf libglib2.0-dev

# Generate configure script
$ autoreconf -i

# Configure the program
$ ./configure

# Compile the program
$ make
```

# Building a Debian package

```bash
# Install compiler, packaging tools, and dependencies
$ sudo apt-get install build-essential dh-autoreconf fakeroot devscripts git-buildpackage libglib2.0-dev

# Build the package
$ git-buildpackage
```

# Recommended reading

Debian Wiki [BuildingTutorial][debwiki-building-tutorial] article.

[autotools]: https://en.wikipedia.org/wiki/GNU_Build_System
[debwiki-building-tutorial]: https://wiki.debian.org/BuildingTutorial
[glib]: https://en.wikipedia.org/wiki/GLib
