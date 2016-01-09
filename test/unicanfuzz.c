#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "linux/can.h"
#include "lib.h"

#include "rng64.h"

#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#if DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

/* http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html */
#define crContParam     void **crParam
#define crBeginContext  struct crContextTag { int crLine
#define crEndContext(x) } *x = (struct crContextTag *)*crParam
#define crBegin(x)      if(!x) {x=*crParam=calloc(1,sizeof(*x)); x->crLine=0;}\
                        if (x) switch(x->crLine) { case 0:;
#define crFinish(z)     } free(*crParam); *crParam=0; return (z)
#define crFinishV       } free(*crParam); *crParam=0; return
#define crReturn(z)     \
        do {\
            ((struct crContextTag *)*crParam)->crLine=__LINE__;\
            return (z); case __LINE__:;\
        } while (0)
#define crReturnV       \
        do {\
            ((struct crContextTag *)*crParam)->crLine=__LINE__;\
            return; case __LINE__:;\
        } while (0)
#define crStop(z)       do{ free(*crParam); *crParam=0; return (z); }while(0)
#define crStopV         do{ free(*crParam); *crParam=0; return; }while(0)
#define crContext       void *
#define crAbort(ctx)    do { free (ctx); ctx = 0; } while (0)

static const uint16_t crc16tab[] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

struct Global {
	int ext;
	int mix;
	unsigned count;
	unsigned gap;
	unsigned mtu;
	unsigned nodes;
	unsigned prob;
	uint64_t seed;
	const char *ifname;
};

struct Global G;

/**INDENT-OFF**/
static void usage(const char *prg)
{
	printf("Usage: %s [options] <ifname>\n", prg);
	printf("Options:\n");
	printf("\n");
	printf("  -e\t\t(generate extended (EFF) CAN frames, default is 'no')\n");
	printf("  -g <ms>\t(inter-frame gap in milli seconds, default is %d ms)\n", G.gap);
	printf("  -m\t\t(generate mix of SFF and EFF CAN frames, default is 'no')\n");
	printf("  -M <bytes>\t(MTU for \"long\" UNICAN messages, default is %d bytes)\n", G.mtu);
	printf("  -n <count>\t(terminate after <count> UNICAN frames, default infinitive)\n");
	printf("  -N <number>\t(<number> of simultaneous issuers, default is %d)\n", G.nodes);
	printf("  -p <percent>\t(specify <percent> of long UNICAN messages, default is %d%%)\n", G.prob);
	printf("  -s <seed>\t(specify seed to <seed> for PRNG, default is %llu)\n", G.seed);
	printf("\n");

	exit(1);
}
/**INDENT-ON**/

static void print_canframe(struct canfd_frame *pf)
{
	int n = 0;
	char buf[1024];
	static float f = 0.0f;
	n = sprintf(buf, "(%.3f) %s ", f += 0.001 * G.gap, G.ifname);
	sprint_canframe(&buf[n], pf, 1, 8);
	fprintf(stdout, "%s\n", buf);
	fflush(stdout);
}

#define CONTEXT	int i;int ext;int prob;int dlc;uint16_t len;uint16_t crc;\
struct canfd_frame frame;

struct gx {
	int crLine;
	 CONTEXT;
};

struct canfd_frame *generator(crContParam)
{
	crBeginContext;
	CONTEXT;
	crEndContext(c);

	crBegin(c);

	if (c->ext) {
		c->frame.can_id &= CAN_EFF_MASK;
		c->frame.can_id |= CAN_EFF_FLAG;
		c->frame.can_id &= ~(1 << 28);
	} else {
		c->frame.can_id &= CAN_SFF_MASK;
		c->frame.can_id &= ~(1 << 10);
	}

	if (c->prob) {
		do {
			c->frame.data[0] = rng64();
			c->frame.data[1] = rng64();
		} while ((c->frame.data[0] == 0xfe
			  && c->frame.data[1] == 0xff));
		c->frame.len = 2 + rng64() % 7;
	} else {
		c->frame.data[0] = 0xfffe % 256;
		c->frame.data[1] = 0xfffe / 256;
		c->frame.data[2] = rng64() % 256;
		c->frame.data[3] = rng64() / 256;
		c->frame.data[4] = (c->len + 2) % 256;
		c->frame.data[5] = (c->len + 2) / 256;
		c->frame.len = 8;
	}

	crReturn(&c->frame);

	if (!c->prob) {
		if (c->ext)
			c->frame.can_id |= 1 << 28;	/* data */
		else
			c->frame.can_id |= 1 << 10;	/* data */

		for (c->i = 0; c->i < c->len; c->i++) {
			uint8_t *byte = &c->frame.data[c->i % 8];
			uint16_t *crc = &c->crc;
			*byte = c->i;
			c->dlc++;
			*crc = (*crc << 8) ^ crc16tab[(*crc >> 8) ^ *byte];
			if (c->dlc == 8) {
				c->frame.len = c->dlc;
				c->dlc = 0;
				crReturn(&c->frame);
			}
		}

		if (c->i == c->len) {
			if (c->dlc < 8) {
				c->frame.data[c->dlc] = c->crc % 256;
				c->dlc++;
			}
		}
		if (c->dlc == 8) {
			c->frame.len = c->dlc;
			c->dlc = 0;
			crReturn(&c->frame);
		}
		c->i++;
		if (c->i == (c->len + 1)) {
			if (c->dlc < 8) {
				c->frame.data[c->dlc] = c->crc / 256;
				c->dlc++;
				c->frame.len = c->dlc;
			}
			crReturn(&c->frame);
		}
	}

	crFinish(NULL);
}

int main(int argc, char *argv[])
{
	int i;
	int opt;
	struct gx **z;
	struct canfd_frame *pf;

	G.count = UINT32_MAX;
	G.gap = 1;
	G.mtu = 64;
	G.nodes = 1;
	G.prob = 20;
	G.seed = 1ULL;

	while ((opt = getopt(argc, argv, "eM:mN:n:p:s:")) != -1) {
		switch (opt) {
		case 'e':
			G.ext = 1;
			break;
		case 'm':
			G.mix = 1;
			break;
		case 'M':
			G.mtu = atoi(optarg);
			break;
		case 'n':
			G.count = atoi(optarg);
			break;
		case 'N':
			G.nodes = atoi(optarg);
			break;
		case 'p':
			G.prob = atoi(optarg);
			break;
		case 's':
			G.seed = atoll(optarg);
			break;
		default:
			usage(argv[0]);
			break;
		}
	}

	if (argc == optind || (argc - optind) != 1)
		usage(argv[0]);

	G.ifname = argv[argc - 1];

	if (G.mtu > 65533) {
		fprintf(stderr, "Error: MTU greater than 65533 bytes!\n");
		return 1;
	}

	if (!G.nodes)
		return 0;

	seed_rng64(G.seed);

	z = calloc(G.nodes, sizeof(void *));
	if (!z) {
		fprintf(stderr, "Out of memory!\n");
		return 1;
	}

	for(i=0;i<argc;i++){
		printf("%s%c", argv[i], i+1==argc? '\n' : ' ');
	}

	while (G.count > 0) {
		size_t n = rng64() % G.nodes;
		if (!z[n]) {
			z[n] = calloc(1, sizeof(struct gx));
			if (!z[n]) {
				fprintf(stderr, "Out of memory!\n");
				return 1;
			}
			z[n]->crLine = 0;
			if (G.mix) {
				z[n]->ext = rng64() % 2;
			} else {
				z[n]->ext = G.ext;
			}
			z[n]->frame.can_id = rng64();
			z[n]->len = G.mtu ? rng64() % G.mtu : 0;
			switch (G.prob) {
			case 0:
				z[n]->prob = 1;
				break;
			case 100:
				z[n]->prob = 0;
				break;
			default:
				z[n]->prob =
				    (uint32_t) rng64() % (1000 / (G.prob * 10));
				break;
			}
			G.count--;
		}
		pf = generator((void **)&z[n]);
		if (pf)
			print_canframe(pf);
	}

	for (i = 0; i < G.nodes; i++) {
		while (z[i]) {
			pf = generator((void **)&z[i]);
			if (pf)
				print_canframe(pf);
		}
	}

	if (z)
		free(z);

	return 0;
}
