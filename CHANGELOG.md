# Changelog

## 0.1.1

- Store the unchanged canonical 8-by-32 surface in 128 bytes of packed 4-bit production data instead of 264 bytes of ASCII hex.
- Keep transform semantics, the public API, inverse matrix, and binary outputs unchanged; the test oracle retains an independent ASCII surface.
- Validation passed for GCC Debug/Release, Clang Debug/Release, GCC ASan/UBSan, and GitHub Actions CI. The ESP32-S3 firmware built with ESP-IDF 5.5.1; hardware execution was unavailable because COM37 could not be opened.
- Measured x86-64 `-Os -ffreestanding` read-only object data decreased by 136 bytes with GCC 13.3 (8456 to 8320) and Clang 18.1 (8504 to 8368); `.data` and `.bss` remained zero. Complete application and primitive figures are documented in README.

## 0.1.0

- Initial standalone C17 release.
- Provides the canonical trajectory-based 256-bit GF(2) transform and exact inverse.
- Includes native tests and a buildable basic example.
