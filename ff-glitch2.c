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
#define SH16MAX (2 * UINT8_MAX)
#define GLITCHPX() randn(SH16MAX)
#define INVERT(v) htons(SH16MAX - ntohs(v))

// Probability.
// TODO: Figure out how to make the number of glitching related
// to the amount of pixels.
#define GLITCHES() (40 + randn(50))

#define GLITCH_INVERT      10
#define GLITCH_INVERT_PXM  1

#define GLITCH_SKIP        70
#define GLITCH_SKIP_PXM    1

#define GLITCH_CORRUPT     50
#define GLITCH_CORRUPT_PXM 4

#define GLITCH_SWAP        20
#define GLITCH_SWAP_PXM    4

#define GLITCH_COMPENSATEC 8
#define GLITCH_COMPENSATER (GLITCH_COMPENSATEC * 16)
#define GLITCH_COMPENSATEM ((skipped / GLITCH_COMPENSATEC))

#define GLITCHN (GLITCH_INVERT + GLITCH_SKIP + GLITCH_CORRUPT + GLITCH_SWAP)
#define SEL(i, name) if (n < (name)) return (i); n -= (name)
static inline unsigned int selglitch() {
	unsigned int n = randn(GLITCHN);
	SEL(0, GLITCH_INVERT);
	SEL(1, GLITCH_SKIP);
	SEL(2, GLITCH_CORRUPT);
	return 3;
}

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int main(int argc, char* argv[]) {
	// TODO: parse argv[1] for range;
	unsigned int glitches = GLITCHES();

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
	unsigned int skipped = 0;
	int glitchpx = 0;
	unsigned int gt = 0;
	pixel* px = (pixel*) buf;

	unsigned int glm; // selector

	// corrupter
	unsigned short gv = GLITCHPX();
	unsigned char gp = randn(2);

	// swapper
	unsigned short gs = randn(2);

	for(pxn = 0; pxn < end; pxn++) {
		if (randn(end / glitches) == 0) {
			// glitch it. glitch it hard.
			gt = selglitch();
			switch (gt) {
			case 0: // invert
				glm = GLITCH_INVERT_PXM;
				break;
			case 1: // skip
				glm = GLITCH_SKIP_PXM;
				break;
			case 2: // corrupt
				glm = GLITCH_CORRUPT_PXM;
				break;
			case 3: // swap
				glm = GLITCH_SWAP_PXM;
				break;
			}
			glitchpx = randn((end * glm) / glitches) + 1;
		}

		if (glitchpx) {
			switch (gt) {
			case 0: // invert
				bread(8);
				px->r = INVERT(px->r);
				px->g = INVERT(px->g);
				px->b = INVERT(px->b);
				bwrite(buf, 8);
				glitchpx--;
				break;
			case 1: // skip reading and trash
				skipped++;
				px->r = GLITCHPX();
				px->g = GLITCHPX();
				px->b = GLITCHPX();
				bwrite(buf, 8);
				glitchpx--;
				break;
			case 2: // glitch
				glitchpx--;
				if (!glitchpx) { // randomize next
					gp = randn(2);
					gv = GLITCHPX();
				}
				bread(8);
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
				bwrite(buf, 8);
				break;
			case 3: // swap
				glitchpx--;
				if (!glitchpx) { // randomize next
					gs = randn(2);
				}
				bread(8);
				pixel tpx = *px;
				switch (gs) {
				case 0: // swap r with g;
					px->r = tpx.g;
					px->g = tpx.r;
				case 1: // swap r with b;
					px->r = tpx.b;
					px->b = tpx.r;
				case 2: // swap b with g;
					px->g = tpx.b;
					px->b = tpx.g;
				}
				bwrite(buf, 8);
				break;
			}
			continue;
		}

		// copy and compensate.
		bread(8);
		bwrite(buf, 8);
	}
	// read skipped things
	for (; skipped > 0; skipped--) {
		bread(8);
	}

	return 0;
}
