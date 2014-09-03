conffile = ${sysconfdir}/mausberryswitch.conf

AM_CFLAGS = \
	$(LIBCONFIG_CFLAGS) \
	-DCONFFILE=\"$(conffile)\" \
	-g \
	-Wall \
	-pipe
AM_LDFLAGS = $(LIBCONFIG_LIBS)

bin_PROGRAMS = mausberry-switch
mausberry_switch_SOURCES = switch.c
