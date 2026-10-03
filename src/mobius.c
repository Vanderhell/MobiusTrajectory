#include "mobius.h"

#include <stddef.h>

extern const uint8_t
    mobius_inverse_rows_v1[MOBIUS_BITS][MOBIUS_STATE_BYTES];

/* Rows are stored as high-to-low hex text, preserving the historical surface. */
static const char surface[8][33] = {
    "481E081FFDAB8273DA10C46B9C4C4EED",
    "FC7C544B1173BBF3295C6715178DFC18",
    "787E58B8248E651B14676D3BA604C804",
    "CBA702E69866206E462F81E0336FED5E",
    "0BFD4D2AE8A55FA92799264E8919ADDF",
    "A068C3D6FCC77FAFC1E7B20810A43E49",
    "F95CD1BBF3280124DBDBBDB541ABCC7F",
    "A275EA898D21D192BFAE1C3A2E64C8E4"
};

static uint8_t nibble(char c)
{
    return (uint8_t)(c <= '9' ? c - '0' : c - 'A' + 10);
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
                uint8_t value = nibble(surface[y][x]);
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
