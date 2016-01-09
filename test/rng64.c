/* http://compgroups.net/comp.lang.c/random-64-bit-number-in-stdc/2044618 */

#include "rng64.h"

static uint64_t rng_a, rng_b, rng_c, rng_counter;
void seed_rng64(uint64_t seed)
{
	int i;
	rng_a = rng_b = rng_c = seed;
	rng_counter = 1;
	for (i = 0; i < 12; i++)
		rng64();
}

uint64_t rng64()
{
	uint64_t tmp = rng_a + rng_b + rng_counter++;
	rng_a = rng_b ^ (rng_b >> 12);
	rng_b = rng_c + (rng_c << 3);
	rng_c = ((rng_c << 25) | (rng_c >> 39)) + tmp;
	return tmp;
}
