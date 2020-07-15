#!/usr/bin/env bash
set -ex

mkdir -p /tmp/installdir

autoreconf -i -f
./configure --prefix=/usr --sysconfdir=/etc
make
make check
make DESTDIR=/tmp/installdir install-strip

fpm \
	-f \
	-s dir \
	-t deb \
	-n mausberry-switch \
	-v 0.7 \
	-C /tmp/installdir \
	-p mausberry-switch_VERSION_ARCH.deb \
	-d "libglib2.0-0 >= 2.3" \
	--deb-systemd data/mausberry-switch.service \
	usr/bin etc
