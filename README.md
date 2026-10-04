# 6502-Emulator-C++20

A cycle-accurate MOS 6502 CPU emulator core written in C++20.

**Status:** Work in progress.

## Currently Implemented

- Core execution loop and cycle-accurate step logic.
- Memory bus interface (`Membar`).
- Standard reset sequence initialization (`$FFFC`).
- Basic addressing modes: Immediate, Zeropage, Zeropage X, Absolute.
- Core instructions: `LDA`, `JSR`.

## Build

Requires C++20 compatible compiler and CMake.

```bash
mkdir build && cd build
cmake ..
make
```

## Documentation

See `Notes.md` for technical design notes and execution logic details.