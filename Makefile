CC ?= cc
CFLAGS ?= -Os
CPPLAGS += -pedantic -Wall
LDFLAGS += -lm

DESTDIR ?= /usr/local

BINS=ff-glitch ff-glitch2

all: $(BINS)

.c:
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

install: $(BINS)
	install $(BINS) $(DESTDIR)/bin

clean:
	rm -f $(BINS)
