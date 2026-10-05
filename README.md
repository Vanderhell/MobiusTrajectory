# MobiusTrajectory

A reversible 256-bit GF(2) transform derived from trajectories over a discrete Möbius surface, implemented in portable C17.

## Overview

The primitive applies one fixed linear mapping to 32-byte states. Its trajectory construction has 256 input states and yields a canonical 256-by-256 binary matrix of rank 256, so the mapping has an exact inverse.

## Properties

- Fixed canonical 8-by-32 surface and 256 trajectory states.
- Linear transform over `GF(2)^256`, with a verified full-rank canonical matrix.
- Forward and exact inverse; in-place operation is supported.
- No context, initialization call, heap, persistent mutable state, or mutable global state.
- This transform is not a cryptographic primitive.

## API

Include `mobius.h`. Call `mobius_forward(output, input)` or `mobius_inverse(output, input)` with 32-byte buffers. Either function returns `false` for a null buffer and supports `output == input`. State bit `i` is bit `i % 8` of byte `i / 8` (least-significant bit first), independent of host endianness.

## Example

[`examples/basic.c`](examples/basic.c) applies the forward and inverse transform in place. Build it as the `mobius_basic` CMake target.

## Build

Requires CMake and a C17 compiler. CMake explicitly requests ISO C17 without compiler extensions.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For a fresh build directory, choose `-DCMAKE_C_COMPILER=gcc` or `clang` to select a compiler.

## Test

```sh
ctest --test-dir build --output-on-failure
```

The C17 tests reconstruct the canonical matrix independently, verify its rank and inverse, compare all basis vectors, and exercise deterministic differential, inverse, and linearity properties.

## Embedded characteristics

The production core is freestanding-compatible C17. It has no persistent context RAM and uses a 32-byte local input copy for safe aliasing. The fixed 8-by-32 canonical surface is stored in production as 128 bytes: two original 4-bit cells per byte, even `x` in the high nibble and odd `x` in the low nibble. Only this representation changed; the surface values, transform, public API, and inverse matrix are unchanged. The test oracle deliberately retains the original ASCII surface and its own conversion.

Host production-object measurements use x86-64 Linux, `-std=c17 -Os -ffreestanding`, and both production source files. Before the packed representation, the surface occupied 264 read-only bytes; it now occupies 128 bytes, a measured reduction of 136 bytes (51.5%). GCC 13.3 reports `.text` 401 → 413 bytes and total read-only data 8456 → 8320 bytes. Clang 18.1 reports `.text` 467 → 468 bytes and read-only data 8504 → 8368 bytes (including its 48-byte `.rodata.cst16`). Both report zero `.data` and `.bss`. Read-only totals include the 8192-byte inverse matrix; the figures are object contributions and exclude linker overhead.

An ESP-IDF 5.5.1 ESP32-S3 `-Os` build with Xtensa GCC 14.2 measured the production surface section at 264 → 128 bytes and the production code/literal sections at 350 → 339 bytes. The inverse table remains 8192 bytes in read-only flash (`.rodata.mobius_inverse_rows_v1` in the link map); production `.data` and `.bss` are zero. The complete test application image measured 256,368 → 256,240 bytes; after the change, ESP-IDF reports a 256,119-byte image, 120,924 bytes of flash data, 74,644 bytes of flash code, 10,032 bytes of application/framework `.data`, 2,112 bytes of `.bss`, and 15,356 bytes of IRAM `.text` plus 1,028 bytes of vectors. These complete-application figures include ESP-IDF and the test harness and are not the library footprint. The ESP-IDF build passed, but no physical-device run was made because COM37 could not be opened. The core does not allocate memory or use operating-system services.

## Algorithm notes

Each of 256 CUT states encodes a 5-bit start column and a 3-bit slope. Its 64-cell trajectory samples 4-bit surface values over the fixed 8-by-32 surface, reflecting its row when horizontal wrapping crosses the Möbius seam. The generated trajectory vectors form the columns of the forward map. The inverse representation is a separate constant dense matrix verified by tests against an independently reconstructed inverse; it is not part of the trajectory definition.

## Compatibility / stability

The canonical surface and bit/byte mapping define the transform's compatibility behavior. The C17 tests check basis outputs and compare production behavior to an independent trajectory reference. Repository version `0.1.1` changes only the production storage representation and does not alter those semantics.

Validation for version 0.1.1 passed with GCC 13.3 Debug and Release, Clang 18.1 Debug and Release, and GCC AddressSanitizer plus UndefinedBehaviorSanitizer. All four CTest tests passed in each configuration, including the 100,000-case differential test against the independent ASCII oracle. The GitHub Actions C17 CI workflow passed for the implementation commit. The ESP32-S3 firmware build passed; serial hardware validation was unavailable because esptool could not open COM37. No device flash operation was performed.

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](LICENSE).
