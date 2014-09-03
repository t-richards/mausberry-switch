dist_sysconf_DATA = mausberryswitch.conf
EXTRA_DIST = mausberryswitch.conf.in

# Files that should be removed, but which Automake does not know.
MOSTLYCLEANFILES = $(dist_sysconf_DATA)

SED_PROCESS = \
    $(AM_V_GEN)$(MKDIR_P) $(dir $@) && \
    $(SED) \
    -e 's|@sysconfdir[@]|$(sysconfdir)|g' \
    < $< > $@

%.conf: %.conf.in Makefile
    $(SED_PROCESS)
