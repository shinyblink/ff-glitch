CC ?= cc
CFLAGS ?= -Os
CPPLAGS += -pedantic -Wall

BINS=ff-glitch

all: $(BINS)

ff-glitch: ff-glitch.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o ff-glitch $^

clean:
	rm $(BINS)
