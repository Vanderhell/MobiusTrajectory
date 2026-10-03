#ifndef MOBIUS_H
#define MOBIUS_H
#include <stdbool.h>
#include <stdint.h>
#define MOBIUS_STATE_BYTES 32u
#define MOBIUS_BITS 256u
/* Fixed historical surface. Null buffers return false; input/output may alias. */
bool mobius_forward(uint8_t output[MOBIUS_STATE_BYTES], const uint8_t input[MOBIUS_STATE_BYTES]);
bool mobius_inverse(uint8_t output[MOBIUS_STATE_BYTES], const uint8_t input[MOBIUS_STATE_BYTES]);
#endif
