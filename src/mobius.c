#include "mobius.h"

#include <stddef.h>

extern const uint8_t
    mobius_inverse_rows_v1[MOBIUS_BITS][MOBIUS_STATE_BYTES];

/* Each byte stores the original high-then-low pair of surface nibbles. */
static const uint8_t surface[8][16] = {
    { 0x48, 0x1E, 0x08, 0x1F, 0xFD, 0xAB, 0x82, 0x73, 0xDA, 0x10, 0xC4, 0x6B, 0x9C, 0x4C, 0x4E, 0xED },
    { 0xFC, 0x7C, 0x54, 0x4B, 0x11, 0x73, 0xBB, 0xF3, 0x29, 0x5C, 0x67, 0x15, 0x17, 0x8D, 0xFC, 0x18 },
    { 0x78, 0x7E, 0x58, 0xB8, 0x24, 0x8E, 0x65, 0x1B, 0x14, 0x67, 0x6D, 0x3B, 0xA6, 0x04, 0xC8, 0x04 },
    { 0xCB, 0xA7, 0x02, 0xE6, 0x98, 0x66, 0x20, 0x6E, 0x46, 0x2F, 0x81, 0xE0, 0x33, 0x6F, 0xED, 0x5E },
    { 0x0B, 0xFD, 0x4D, 0x2A, 0xE8, 0xA5, 0x5F, 0xA9, 0x27, 0x99, 0x26, 0x4E, 0x89, 0x19, 0xAD, 0xDF },
    { 0xA0, 0x68, 0xC3, 0xD6, 0xFC, 0xC7, 0x7F, 0xAF, 0xC1, 0xE7, 0xB2, 0x08, 0x10, 0xA4, 0x3E, 0x49 },
    { 0xF9, 0x5C, 0xD1, 0xBB, 0xF3, 0x28, 0x01, 0x24, 0xDB, 0xDB, 0xBD, 0xB5, 0x41, 0xAB, 0xCC, 0x7F },
    { 0xA2, 0x75, 0xEA, 0x89, 0x8D, 0x21, 0xD1, 0x92, 0xBF, 0xAE, 0x1C, 0x3A, 0x2E, 0x64, 0xC8, 0xE4 }
};

static uint8_t surface_get(uint32_t y, uint32_t x)
{
    uint8_t packed = surface[y][x >> 1];
    if ((x & 1u) == 0u) {
        return (uint8_t)(packed >> 4);
    }
    return (uint8_t)(packed & UINT8_C(0x0F));
}

static bool get(const uint8_t *value, uint32_t bit)
{
    return (value[bit / 8u] & (uint8_t)(1u << (bit % 8u))) != 0u;
}

static void flip(uint8_t *value, uint32_t bit)
{
    value[bit / 8u] ^= (uint8_t)(1u << (bit % 8u));
}

static bool parity8(uint8_t value)
{
    value ^= (uint8_t)(value >> 4);
    value ^= (uint8_t)(value >> 2);
    value ^= (uint8_t)(value >> 1);
    return (value & 1u) != 0u;
}

bool mobius_forward(uint8_t output[MOBIUS_STATE_BYTES],
                    const uint8_t input[MOBIUS_STATE_BYTES])
{
    uint32_t cut;
    uint32_t step;
    size_t index;
    uint8_t input_copy[MOBIUS_STATE_BYTES];

    if (output == NULL || input == NULL) {
        return false;
    }

    for (index = 0u; index < MOBIUS_STATE_BYTES; ++index) {
        input_copy[index] = input[index];
    }
    for (index = 0u; index < MOBIUS_STATE_BYTES; ++index) {
        output[index] = 0u;
    }

    for (cut = 0u; cut < MOBIUS_BITS; ++cut) {
        if (get(input_copy, cut)) {
            uint32_t x = cut & 31u;
            uint32_t y = 0u;
            uint32_t slope = cut >> 5;

            for (step = 0u; step < 64u; ++step) {
                uint8_t value = surface_get(y, x);
                if ((step & 1u) == 0u) {
                    output[step / 2u] ^= (uint8_t)(value << 4);
                } else {
                    output[step / 2u] ^= value;
                }

                {
                    uint32_t next_y = (y + slope + (x & 1u)) & 7u;
                    uint32_t next_x = x + 1u;
                    if (next_x >= 32u) {
                        next_x = 0u;
                        next_y = 7u - next_y;
                    }
                    x = next_x;
                    y = next_y;
                }
            }
        }
    }

    return true;
}

bool mobius_inverse(uint8_t output[MOBIUS_STATE_BYTES],
                    const uint8_t input[MOBIUS_STATE_BYTES])
{
    uint32_t row;
    size_t index;
    uint8_t input_copy[MOBIUS_STATE_BYTES];

    if (output == NULL || input == NULL) {
        return false;
    }

    for (index = 0u; index < MOBIUS_STATE_BYTES; ++index) {
        input_copy[index] = input[index];
    }
    for (index = 0u; index < MOBIUS_STATE_BYTES; ++index) {
        output[index] = 0u;
    }

    for (row = 0u; row < MOBIUS_BITS; ++row) {
        size_t byte_index;
        bool parity = false;

        for (byte_index = 0u; byte_index < MOBIUS_STATE_BYTES; ++byte_index) {
            parity = parity ^ parity8((uint8_t)(
                mobius_inverse_rows_v1[row][byte_index] &
                input_copy[byte_index]));
        }
        if (parity) {
            flip(output, row);
        }
    }

    return true;
}
