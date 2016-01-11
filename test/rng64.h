#ifndef RNG64_H_
#define RNG64_H_
#include <stdint.h>
void seed_rng64(uint64_t seed);
uint64_t rng64();
#endif /* RNG64_H_ */
