prefix = /

all: src/networkwatchdogd

src/networkwatchdogd: src/networkwatchdogd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lm

install: src/networkwatchdogd
	install -D src/networkwatchdogd $(DESTDIR)$(prefix)bin/networkwatchdogd
clean:
	-rm -f src/networkwatchdogd
distclean: clean

uninstall:
	-rm -f $(DESTDIR)$(prefix)bin/networkwatchdogd

.PHONY: all install clean distclean uninstall
