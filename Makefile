CC ?= cc
CFLAGS ?= -Os
CPPLAGS += -pedantic -Wall

BINS=ff-glitch ff-glitch2

all: $(BINS)

ff-glitch: ff-glitch.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o ff-glitch $^
ff-glitch2: ff-glitch2.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o ff-glitch2 $^

clean:
	rm -f $(BINS)
