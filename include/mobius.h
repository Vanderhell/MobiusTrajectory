#ifndef MOBIUS_H
#define MOBIUS_H
#include <stdbool.h>
#include <stdint.h>
#define MOBIUS_STATE_BYTES 32u
#define MOBIUS_BITS 256u
typedef struct { uint8_t inverse_rows[MOBIUS_BITS][MOBIUS_STATE_BYTES]; } mobius_ctx_t;
/* Fixed historical 8x32 surface; each cell contributes its hexadecimal nibble. */
bool mobius_init(mobius_ctx_t *ctx);
bool mobius_forward(uint8_t output[MOBIUS_STATE_BYTES], const uint8_t input[MOBIUS_STATE_BYTES]);
bool mobius_inverse(uint8_t output[MOBIUS_STATE_BYTES], const uint8_t input[MOBIUS_STATE_BYTES], const mobius_ctx_t *ctx);
#endif
