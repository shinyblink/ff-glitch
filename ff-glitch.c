// ┳━┓┳━┓  ┏━┓┳  o┏┓┓┏━┓┳ ┳
// ┣━ ┣━ ━━┃ ┳┃  ┃ ┃ ┃  ┃━┫
// ┇  ┇    ┇━┛┇━┛┇ ┇ ┗━┛┇ ┻
// ff-glitch: the farbfeld glitcher.
// usage: <farbfeld source> | ff-glitch | <farbfeld sink>
// thanks to xero for the idea! you rock.
// made by vifino. ISC (C) vifino 2018

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <arpa/inet.h>

// TODO: replace ntohl

static uint32_t w, h;
static unsigned char buf[64] = {0};
static unsigned char img;
static struct timeval tv;

typedef struct {
	unsigned short r, g, b, a;
} pixel;

// IO helpers.
static inline void bread(int bytes) {
	if (!fread(buf, bytes, 1, stdin)) {
		fprintf(stderr, "wanted more bytes, didn't get any?\n");
		exit(1);
	}
}

static inline void bwrite(unsigned char* buffer, size_t bytes) {
	if (!fwrite(buffer, bytes, 1, stdout)) {
		fprintf(stderr, "write failed.\n");
		exit(1);
	}
}

// random helper, random from 0-n, inclusive.
// tries to get evenly spread out random.
unsigned int randn(unsigned int n) {
	if (!n) return 0;
	if (n == RAND_MAX) return rand();
	n++;
	long end = RAND_MAX / n;
	end *= n;
	int r;
	while ((r = rand()) >= end);
	return r % n;
}

// Glitch a short.
#define GLITCHPX() randn(2 * UINT8_MAX)

int main(int argc, char* argv[]) {
	// TODO: parse argv[1] for range;
	unsigned int glitches = 40 + randn(50);

	bread(8);
	if (strcmp(buf, "farbfeld") != 0) {
		fprintf(stderr, "stdin is not a farbfeld image?\n");
		return 1;
	}
	bwrite(buf, 8);

	// seed like a dumbass.
	if (gettimeofday(&tv, NULL)) {
		fprintf(stderr, "failed to get the time? that's messed up.\n");
		return 2;
	}
	srand(tv.tv_usec);

	// parse width and height
	bread(8);
	uint32_t w = ntohl(*(uint32_t*)buf);
	uint32_t h = ntohl(*((uint32_t*)(buf + 4)));
	if (!w && !h) {
		fprintf(stderr, "image size has zero dimension? %i*%i\n", w, h);
		return 1;
	}
	bwrite(buf, 8);

	unsigned int pxn;
	unsigned int end = w*h;
	unsigned int trash = 0;
	unsigned int skipped = 0;
	int glitchpx = 0;
	unsigned short gv;
	unsigned char gp = 0;
	pixel glitchp;
	for(pxn = 0; pxn < end; pxn++) {
		if (randn(end / glitches) == 0) {
			// glitch it. glitch it hard.
			trash = randn(end / 10 / glitches);
			if (randn(1) == 0) {
				glitchpx = randn((end * 4) / glitches);
				gp = randn(2);
				gv = GLITCHPX();
			}
		}

		if (trash) {
			// trash it.
			pxn = pxn + trash - 1;
			while (trash > 1) {
				if (randn(1) == 0) {
					bread(8); // throw away
				} else {
					skipped++;
				}
				pixel* px = (pixel*) buf;
				px->r = GLITCHPX();
				px->g = GLITCHPX();
				px->b = GLITCHPX();
				trash--;
				bwrite(buf, 8);
			}
		}

		bread(8);
		if (glitchpx) {
			pixel* px = (pixel*) buf;
			if ((glitchpx - 1) >= 0) {
				switch (gp) {
				case 0:
					px->r = gv;
					break;
				case 1:
					px->g = gv;
					break;
				case 2:
					px->b = gv;
					break;
				}
				glitchpx--;
			}
			if (glitchpx < 0) glitchpx = 0;
		}

		bwrite(buf, 8);
	}
	for (; skipped > 0; skipped--)
		bread(8);

	return 0;
}
