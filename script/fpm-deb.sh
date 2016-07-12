#!/usr/bin/env bash
set -ex

mkdir -p /tmp/installdir

autoreconf -i
./configure --prefix=/usr --sysconfdir=/etc
make
make DESTDIR=/tmp/installdir

fpm \
	-s dir \
	-t deb \
	-n mausberry-switch \
	-v 0.6 \
	-C /tmp/installdir \
	-p mausberry-switch_VERSION_ARCH.deb \
	-d "libglib2.0-0 >= 2.3" \
	usr/bin etc
