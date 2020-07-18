# Building & packaging from source

This guide covers compilation and package assembly for Raspberry Pi OS ("Raspbian") based on Debian 10 (Buster).

Other distributions are not officially supported.

# Requirements

 - [Autotools][autotools]
 - C compiler
 - [GLib][glib] development/runtime libraries
 - [Ruby][ruby] interpreter with development headers

# Before you begin

The following commands are intended to be run directly on your Raspberry Pi system.

Cross-compile at your own risk.

# Building and installing a debian package

```bash
# Install compiler and dependencies
$ sudo apt-get install build-essential dh-autoreconf git libglib2.0-dev ruby ruby-dev

# Install packaging tools
$ sudo gem install --no-document fpm

# Clone repository
$ git clone https://github.com/t-richards/mausberry-switch.git
$ cd mausberry-switch

# Build the package
$ ./script/build

# Install the package
$ sudo dpkg -i mausberry-switch*.deb
$ sudo apt-get -f install
```

[autotools]: https://en.wikipedia.org/wiki/GNU_Build_System
[glib]: https://en.wikipedia.org/wiki/GLib
[ruby]: https://www.ruby-lang.org/
