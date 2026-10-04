import <iostream>;
#include <cstdint>
import <array>;
import <ranges>;
import <algorithm>;
import <iomanip>;
import <format>;

using Byte = std::uint8_t;
using Word = std::uint16_t;
struct Memory {
  static constexpr std::uint32_t MAX_SIZE{1024 * 64};
  std::array<Byte, MAX_SIZE> Data{};

  void init() { std::ranges::fill(Data, 0); }

  // Read Byte Function
  Byte operator[](std::uint32_t Address) const {
    // Assume the address is less than MAX_SIZE
    return Data[Address];
  }

  // Write Byte(Return as a reference to that part in memory that I can write
  // to)
  Byte &operator[](std::uint32_t Address) {
    // Assume the address is less than MAX_SIZE
    return Data[Address];
  }

  // Write 2 bytes
  void WriteWord(Word Value, std::uint32_t Address,
                 std::uint32_t &NumOfCycles) {
    Data[Address] = Value & 0xFF; // The least significant byte
    Data[Address] = (Value >> 8); // The most significant byte
    NumOfCycles -= 2;
  }
};

struct CPU {

  // Program Counter & Stack Pointer
  Word PC;
  Word SP;

  // Registers
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

  // State Reset
  void Reset(Memory &Membar) {
    PC = 0xFFFC;
    SP = 0x0100;
    C = Z = I = D = B = V = N = 0;
    A = X = Y = 0;
    Membar.init();
  }

  Byte FetchByte(std::uint32_t &NumOfCycles, Memory &Membar) {
    Byte Data{Membar[PC]};
    PC++;
    NumOfCycles--;
    return Data;
  }
  Byte ReadByte(std::uint32_t &NumOfCycles, const Byte &Address,
                Memory &Membar) {
    Byte Data{Membar[Address]};
    NumOfCycles--;
    return Data;
  }
  Word FetchWord(std::uint32_t &NumOfCycles, Memory &Membar) {
    Word Data{Membar[PC]};
    PC++;
    NumOfCycles--;

    Data |= (Membar[PC] << 8);
    PC++;
    NumOfCycles -= 2;

    // Handle endianness
    // Swap Bytes
    // if(IS_BIG_ENDIAN)
    // then SwapByteInWord(Data)
    return Data;
  }

  static constexpr Byte INST_LDA_IMM{0xA9};  // LDA Immediate Addressing Mode
  static constexpr Byte INST_LDA_ZRP{0xA5};  // LDA Zero Page Addressing Mode
  static constexpr Byte INST_LDA_ZRPX{0xB5}; // LDA Zero Page X Addressing Mode
  static constexpr Byte INST_JSR{0x20};      // LDA Zero Page X Addressing Mode
  void LDAStatusSet() {
    Z = (A == 0);
    N = (A & 0b10000000) >> 7;
  }
  void Execute(std::uint32_t NumOfCycles, Memory &Membar) {
    while (NumOfCycles > 0) {
      Byte Instruction{FetchByte(NumOfCycles, Membar)};
      switch (Instruction) {
      case INST_LDA_IMM: {
        Byte Value{FetchByte(NumOfCycles, Membar)};
        A = Value;
        LDAStatusSet();
      } break;
      case INST_LDA_ZRP: {
        Byte ZeroPageAddress{FetchByte(NumOfCycles, Membar)};
        A = ReadByte(NumOfCycles, ZeroPageAddress, Membar);
        LDAStatusSet();
      } break;
      case INST_LDA_ZRPX: {
        Byte ZeroPageAddress{FetchByte(NumOfCycles, Membar)};
        ZeroPageAddress += X;
        NumOfCycles--;
        A = ReadByte(NumOfCycles, ZeroPageAddress, Membar);
        LDAStatusSet();
      } break;
      case INST_JSR: {
        Word SubroutineAddress{FetchWord(NumOfCycles, Membar)};
        Membar.WriteWord(PC - 1, SP, NumOfCycles);
        PC = SubroutineAddress;
        NumOfCycles--;
      } break;

      default:
        [[unlikely]] {
          std::cout << "Unable to handle instruction: "
                    << std::format("{:#04X}\n", Instruction);
        }
        break;
      }
    }
  }
};

int main(int argc, char *argv[]) {
  Memory bar1;
  CPU sample;
  // Try to inline a small program
  sample.Reset(bar1);
  bar1[0xFFFC] = CPU::INST_JSR;
  bar1[0xFFFD] = 0x77;
  bar1[0xFFFE] = 0x77;
  bar1[0x7777] = CPU::INST_LDA_IMM;
  bar1[0x7778] = 0x89;
  // EndProg
  sample.Execute(9, bar1);
  return 0;
}