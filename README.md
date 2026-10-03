# Mobius Trajectory

Native C17 implementation of the frozen historical trajectory transform. Eight rows by 32 columns hold one 4-bit value per cell. Each CUT encodes a 5-bit start column and 3-bit slope; its 64-cell path starts at row zero, advances by `slope + (x & 1)`, and reflects the row at each horizontal seam. The 256 path vectors form the columns of a GF(2) map; initialization verifies full rank and stores its inverse.

State bit `i` is bit `i % 8` of byte `i / 8` (least-significant bit first). Trajectory samples are packed high nibble first into the 32-byte output. Surface constants preserve the original 8x32 baseline. Build and run the native C17 tests with CMake and CTest.
