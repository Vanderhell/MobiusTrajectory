#include "mobius.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    uint8_t state[MOBIUS_STATE_BYTES] = {0u};
    uint8_t original[MOBIUS_STATE_BYTES];
    size_t index;

    state[0] = 1u;
    memcpy(original, state, sizeof state);
    if (!mobius_forward(state, state) || !mobius_inverse(state, state)) {
        return 1;
    }
    if (memcmp(state, original, sizeof state) != 0) {
        return 1;
    }

    for (index = 0u; index < sizeof state; ++index) {
        printf("%02x%s", (unsigned int)state[index],
               index + 1u == sizeof state ? "\n" : " ");
    }
    return 0;
}
