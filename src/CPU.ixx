module;

#include <cstdint>

export module CPU_6502;

import <iostream>;
import <array>;
import <ranges>;
import <algorithm>;
import <iomanip>;
import <format>;

export {
    using Byte = std::uint8_t;
    using Word = std::uint16_t;
    struct Memory
    {
        static constexpr std::uint32_t MAX_SIZE{1024 * 64};
        std::array<Byte, MAX_SIZE> Data{};

        void init()
        {
            std::ranges::fill(Data, 0);
        }

        // Read 1 Byte
        Byte operator[](std::uint32_t Address) const
        {
            // Assume the address is less than MAX_SIZE
            return Data[Address];
        }

        // Write 1 Byte

        Byte &operator[](std::uint32_t Address)
        {
            // Assume the address is less than MAX_SIZE
            return Data[Address];
        }

        void WriteWord(Word Value, std::uint32_t Address, std::uint32_t &NumOfCycles)
        {
            Data[Address] = Value & 0xFF;
            Data[Address + 1] = (Value >> 8);
            NumOfCycles -= 2;
        }
    };

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

        void Reset(Memory &Mbar)
        {
            PC = 0xFFFC;
            SP = 0x0100;
            C = Z = I = D = B = V = N = 0;
            A = X = Y = 0;
            Mbar.init();
        }

        Byte FetchByte(std::uint32_t &NumofCycles, Memory &Mbar)
        {
            Byte Data{Mbar[PC]};
            PC++;
            NumofCycles--;
            return Data;
        }

        Word FetchWord(std::uint32_t &NumofCycles, Memory &Mbar)
        {
            Word Data{Mbar[PC]};
            PC++;

            Data |= (Mbar[PC] << 8);
            PC++;
            NumofCycles -= 2;

            // Handle endianness
            // Swap Bytes
            // if(IS_BIG_ENDIAN)
            // then SwapByteInWord(Data)
            return Data;
        }

        Byte ReadByte(std::uint32_t &NumOfCycles, Byte Address, Memory &Mbar)
        {
            Byte Data{Mbar[Address]};
            NumOfCycles--;
            return Data;
        }

        static constexpr Byte INST_LDA_IMM{0xA9};  // LDA Immediate Addressing Mode
        static constexpr Byte INST_LDA_ZRP{0xA5};  // LDA Zero Page Addressing Mode
        static constexpr Byte INST_LDA_ZRPX{0xB5}; // LDA Zero Page X Addressing Mode
        static constexpr Byte INST_JSR{0x20};      // JSR
        void LDAStatusSet()
        {
            Z = (A == 0);
            N = (A & 0b10000000) >> 7;
        }

        // Return the number of cycles that were used
        std::int32_t Execute(std::uint32_t NumOfCycles, Memory &Mbar)
        {
            const std::uint32_t CyclesRequested(NumOfCycles);
            while (NumOfCycles > 0)
            {
                Byte Instruction{FetchByte(NumOfCycles, Mbar)};
                switch (Instruction)
                {
                case INST_LDA_IMM: {
                    Byte Value{FetchByte(NumOfCycles, Mbar)};
                    A = Value;
                    LDAStatusSet();
                }
                break;
                case INST_LDA_ZRP: {
                    Byte ZeroPageAddress{FetchByte(NumOfCycles, Mbar)};
                    A = ReadByte(NumOfCycles, ZeroPageAddress, Mbar);
                    LDAStatusSet();
                }
                break;

                case INST_LDA_ZRPX: {
                    Byte ZeroPageAddress{FetchByte(NumOfCycles, Mbar)};
                    ZeroPageAddress += X;
                    NumOfCycles--;
                    A = ReadByte(NumOfCycles, ZeroPageAddress, Mbar);
                    LDAStatusSet();
                }
                break;

                case INST_JSR: {
                    Word SubroutineAddress{FetchWord(NumOfCycles, Mbar)};
                    Mbar.WriteWord(PC - 1, SP, NumOfCycles);
                    SP += 2;
                    PC = SubroutineAddress;
                    NumOfCycles--;
                }
                break;
                default:
                    [[unlikely]]
                    {
                        std::cout << "Unable to handle instruction: " << std::format("{:#04X}\n", Instruction);
                    }
                    break;
                }
            }
            const std::int32_t NumOfCyclesUsed(CyclesRequested - NumOfCycles);
            return NumOfCyclesUsed;
        }
    };
}