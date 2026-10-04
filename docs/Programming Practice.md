# C++ 20 std::format

### Format String Syntax Rule

$$\text{\{\{ [index] : [[fill]align][sign][\#][0][width][.precision][type] \}\}}$$

### C++20 `std::format` Specifier Reference

| **Specifier**    | **Option / Description**                                     | **Meaning**                                                  | **Example (x = 42 / s = "hello")**                           | **Formatted Output**             |
| ---------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | -------------------------------- |
| **`align`**      | `<` `>` `^`                                                  | Left-align, Right-align, Center-align                        | `std::format("{:<6}", 42)` `std::format("{:>6}", 42)` `std::format("{:^6}", 42)` | `"42    "` `"    42"` `"  42  "` |
| **`fill`**       | Any character before `align`                                 | Character used for padding (default is space)                | `std::format("{:*^6}", 42)`                                  | `"**42**"`                       |
| **`sign`**       | `+` `-` Space                                                | Force `+` for positive numbers Show `-` only for negative numbers (default) Pad positive numbers with a space | `std::format("{:+}", 42)` `std::format("{: }", 42)`          | `"+42"` `" 42"`                  |
| **`#`**          | Alternate form                                               | Adds radix prefix (`0b`, `0o`, `0x`, `0X`), forces decimal point for floating-point | `std::format("{:#x}", 42)` `std::format("{:#06X}", 42)`      | `"0x2a"` `"0X002A"`              |
| **`0`**          | Zero-padding                                                 | Pads with leading zeros within the field width               | `std::format("{:05}", 42)`                                   | `"00042"`                        |
| **`width`**      | Integer                                                      | Minimum field width                                          | `std::format("{:5}", 42)`                                    | `"   42"`                        |
| **`.precision`** | `.` followed by integer                                      | Precision for floating-point numbers or max character length for strings | `std::format("{:.2f}", 3.14159)` `std::format("{:.3}", "hello")` | `"3.14"` `"hel"`                 |
| **`type`**       | **Integer**: `d` (dec), `x`/`X` (hex), `b`/`B` (bin), `o` (oct), `c` (char) **Float**: `f`/`F` (fixed), `e`/`E` (scientific), `g`/`G` (general), `a`/`A` (hex float) | Output representation type                                   | `std::format("{:b}", 10)` `std::format("{:#010b}", 10)` `std::format("{:c}", 65)` | `"1010"` `"0b00001010"` `"A"`    |

# C++ Attributes

Attributes provide unified syntax to give explicit hints to the compiler's optimizer, enforce API safety, and control code generation without affecting runtime semantics.

---

### 1. Branch Prediction Hints: `[[likely]]` / `[[unlikely]]` (C++20)
* **Purpose**: Guides the CPU branch predictor and instruction cache (I-Cache) layout by specifying hot/cold execution paths.
* **Use Case**: Error handling, CPU emulator instruction decoding, rare interrupts/traps.

```c++
if (InterruptRequested) [[unlikely]] {
    HandleInterrupt();
} else [[likely]] {
    ExecuteNextInstruction();
}
```

### 2. API Safety: `[[nodiscard("reason")]]` (C++20 Enhanced)

- **Purpose**: Generates a compiler warning if the return value of a function is ignored. C++20 adds a message string explaining why it must not be discarded.
- **Use Case**: Memory-read functions, resource acquisition, status check operations.

```c++
[[nodiscard("Ignoring memory read result corrupts CPU state")]]
Byte ReadByte(uint32_t Address) {
    return Data[Address];
}
```

### 3. Zero-Cost Abstraction: `[[no_unique_address]]` (C++20)

- **Purpose**: Informs the compiler that an empty struct/class member does not require a distinct memory address, optimizing its storage footprint to 0 bytes (Empty Base Optimization / EBO).
- **Use Case**: Injecting stateless loggers, custom memory allocators, or tag dispatchers into CPU structures.

```c++
struct NullLogger {}; // Empty type

struct CPU {
    [[no_unique_address]] NullLogger logger; // Occupies 0 bytes
    uint16_t PC;
    uint8_t A;
};
```

### 4. Switch Fall-Through Control: `[[fallthrough]]` (C++17)

- **Purpose**: Suppresses implicit `fallthrough` compiler warnings when a `switch-case` block intentionally omits a `break` statement.
- **Use Case**: OpCode instruction decoding sharing microcode logic across multiple instructions.

```c++
switch (OpCode) {
    case INST_NOP_1:
    case INST_NOP_2:
        [[fallthrough]];
    case INST_NOP_3:
        NumOfCycles -= 2;
        break;
}
```

### 5. Compiler Assumption Optimization: `[[assume(expr)]]` (C++23)

- **Purpose**: Asserts an invariant condition directly to the optimizer. Generates no runtime checks, allowing aggressive dead-code elimination and bounds-check stripping.
- **Use Case**: Bounding hardware memory access ranges, eliminating redundant bounds checks in hot loops.

```c++
Byte ReadMemory(uint32_t Address) {
    [[assume(Address < 65536)]]; // Optimizer assumes 16-bit address boundary
    return Data[Address];
}
```

# Notes

When I try to get something to work, I write a bit of inline code, which would be loaded from disk.