CC ?= cc
CFLAGS ?= -Os
CPPLAGS += -pedantic -Wall
LDFLAGS += -lm

PREFIX ?= /usr/local
DESTDIR ?= /

BINS=ff-glitch ff-glitch2

all: $(BINS)

.c:
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

install: $(BINS)
	install -d $(DESTDIR)/$(PREFIX)/bin
	install $(BINS) $(DESTDIR)/$(PREFIX)/bin

clean:
	rm -f $(BINS)
