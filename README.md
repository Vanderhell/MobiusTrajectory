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

The production core is freestanding-compatible C17. It has no persistent context RAM and uses a 32-byte local input copy for safe aliasing. The dense inverse matrix is immutable compile-time data of 8192 bytes in ROM; actual section placement depends on the target toolchain and linker. A host object measurement using `-Os -ffreestanding` on x86-64 Linux reports, for GCC 13.3, 401 bytes of `.text` and 8456 bytes of read-only data; Clang 18.1 reports 467 bytes of `.text` and 8504 bytes of read-only data (including its 48-byte `.rodata.cst16`). Both report zero `.data` and `.bss`. These toolchain-specific object figures include the 8192-byte inverse matrix and exclude linker overhead. The current implementation does not allocate memory or use operating-system services.

## Algorithm notes

Each of 256 CUT states encodes a 5-bit start column and a 3-bit slope. Its 64-cell trajectory samples 4-bit surface values over the fixed 8-by-32 surface, reflecting its row when horizontal wrapping crosses the Möbius seam. The generated trajectory vectors form the columns of the forward map. The inverse representation is a separate constant dense matrix verified by tests against an independently reconstructed inverse; it is not part of the trajectory definition.

## Compatibility / stability

The canonical surface and bit/byte mapping define the transform's compatibility behavior. The C17 tests check basis outputs and compare production behavior to an independent trajectory reference. Repository version `0.1.0` does not alter those semantics.

## License

No license has been selected yet.
