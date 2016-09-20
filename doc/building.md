# Building & packaging from source

This guide covers compilation and package assembly for Debian 7 "Wheezy", and
Debian 8 "Jessie" only. Other distributions are not officially supported.

# Requirements

 - [Autotools][autotools]
 - C compiler
 - [GLib][glib] development/runtime libraries
 - [Ruby][ruby] interpreter with development headers

# Before you begin

The following commands are intended to be run directly in your Raspberry Pi
"Raspbian" system. Cross-compile at your own risk. Assuming you're in the
directory where you cloned this repository:

# Building from source, no packaging

```bash
# Install compiler and dependencies
$ sudo apt-get install build-essential dh-autoreconf libglib2.0-dev ruby

# Generate configure script
$ autoreconf -i -f

# Configure the program
$ ./configure

# Compile the program
$ make

# Install the program on your machine (but seriously, don't actually do this please)
$ make install
```

# Building a Debian package

```bash
# Install all the things above, then
$ sudo apt-get install ruby-dev

# Install packaging tools
$ gem install fpm

# Build the package
$ ./script/build.sh

# Install the package, fix deps if necessary
$ sudo dpkg -i mausberry-switch*.deb
$ sudo apt-get -f install
```

[autotools]: https://en.wikipedia.org/wiki/GNU_Build_System
[glib]: https://en.wikipedia.org/wiki/GLib
[ruby]: https://www.ruby-lang.org/
