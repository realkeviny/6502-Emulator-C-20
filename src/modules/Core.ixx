module;

export module cpu:core;

import std;
import :types;
import :memory;

export namespace MOS6502
{
struct CPU
{
    // Program Counter & Stack Pointer
    Word PC;
    Word SP;

    Byte A; // Accumulator
    Byte X, Y;

    // CPU Status Flag
    Byte C : 1; // Carry
    Byte Z : 1; // Zero Flag
    Byte I : 1; // Interrupt
    Byte D : 1; // Decimal
    Byte B : 1; // Break
    Byte V : 1; // OverFlow
    Byte N : 1; // Negative

    void Reset(Memory &Mbar);
    // Return the number of cycles that were used
    std::int32_t Execute(std::int32_t NumOfCycles, Memory &Mbar);

    Byte FetchByte(std::int32_t &NumofCycles, Memory &Mbar);
    Word FetchWord(std::int32_t &NumofCycles, Memory &Mbar);
    Byte ReadByte(std::int32_t &NumOfCycles, Word Address, Memory &Mbar);
    Word ReadWord(std::int32_t &NumOfCycles, Word Address, Memory &Mbar);

    static constexpr Byte INST_LDA_IMM{0xA9};  // LDA Immediate Addressing Mode
    static constexpr Byte INST_LDA_ZRP{0xA5};  // LDA Zero Page Addressing Mode
    static constexpr Byte INST_LDA_ZRPX{0xB5}; // LDA Zero Page X Addressing Mode
    static constexpr Byte INST_LDA_ABS{0xAD};  // LDA Absolute Addressing Mode
    static constexpr Byte INST_LDA_ABSX{0xBD}; // LDA Absolute X Addressing Mode
    static constexpr Byte INST_LDA_ABSY{0xB9}; // LDA Absolute Y Addressing Mode
    static constexpr Byte INST_LDA_INDX{0xA1}; // LDA Indexed Indirect X Addressing Mode
    static constexpr Byte INST_LDA_INDY{0xB1}; // LDA Indirect Indexed Y Addressing Mode
    static constexpr Byte INST_JSR{0x20};      // JSR

    void LDAStatusSet();
};
} // namespace MOS6502