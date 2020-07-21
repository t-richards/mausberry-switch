AM_CPPFLAGS = \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	$(GLIB_CFLAGS) \
	$(GPIOD_CFLAGS)

bin_PROGRAMS = \
	mausberry-switch

mausberry_switch_LDADD = \
	libmaus.a \
	$(GLIB_LIBS) \
	$(GPIOD_LIBS)

mausberry_switch_SOURCES = \
	main.c

noinst_LIBRARIES = \
	libmaus.a

libmaus_a_SOURCES = \
	maus.c
