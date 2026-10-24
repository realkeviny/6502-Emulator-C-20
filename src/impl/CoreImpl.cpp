module;

module cpu;

import std;

namespace MOS6502
{
void CPU::Reset(Memory &Mbar)
{
    PC = 0xFFFC;
    SP = 0x0100;
    C = Z = I = D = B = V = N = 0;
    A = X = Y = 0;
    Mbar.init();
}

Byte CPU::FetchByte(std::int32_t &NumofCycles, Memory &Mbar)
{
    Byte Data{Mbar[PC]};
    PC++;
    NumofCycles--;
    return Data;
}

Word CPU::FetchWord(std::int32_t &NumofCycles, Memory &Mbar)
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

Byte CPU::ReadByte(std::int32_t &NumOfCycles, Word Address, Memory &Mbar)
{
    Byte Data{Mbar[Address]};
    NumOfCycles--;
    return Data;
}

Word CPU::ReadWord(std::int32_t &NumOfCycles, Word Address, Memory &Mbar)
{
    Byte LSB{ReadByte(NumOfCycles, Address, Mbar)};     // Lower Byte
    Byte HSB{ReadByte(NumOfCycles, Address + 1, Mbar)}; // Higher Byte

    return LSB | (HSB << 8);
}

void CPU::LDAStatusSet()
{
    Z = (A == 0);
    N = (A & 0b10000000) >> 7;
}

// Return the number of cycles that were used
std::int32_t CPU::Execute(std::int32_t NumOfCycles, Memory &Mbar)
{
    const std::int32_t CyclesRequested(NumOfCycles);
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

        case INST_LDA_ABS: {
            Word AbsoluteAddress{FetchWord(NumOfCycles, Mbar)};
            A = ReadByte(NumOfCycles, AbsoluteAddress, Mbar);
        }
        break;

        case INST_LDA_ABSX: {
            Word AbsoluteAddress{FetchWord(NumOfCycles, Mbar)};
            Word AbsoluteAddressX = AbsoluteAddress + X;
            A = ReadByte(NumOfCycles, AbsoluteAddressX, Mbar);
            if (AbsoluteAddressX - AbsoluteAddress >= 0xFF)
            {
                NumOfCycles--;
            }
        }
        break;

        case INST_LDA_ABSY: {
            Word AbsoluteAddress{FetchWord(NumOfCycles, Mbar)};
            Word AbsoluteAddressY = AbsoluteAddress + Y;
            A = ReadByte(NumOfCycles, AbsoluteAddressY, Mbar);
            if (AbsoluteAddressY - AbsoluteAddress >= 0xFF)
            {
                NumOfCycles--;
            }
        }
        break;

        case INST_LDA_INDX: {
            Byte ZeroPageAddress{FetchByte(NumOfCycles, Mbar)};
            ZeroPageAddress += X;
            NumOfCycles--;
            Word ActualAddress{ReadWord(NumOfCycles, ZeroPageAddress, Mbar)};
            A = ReadByte(NumOfCycles, ActualAddress, Mbar);
        }
        break;

        case INST_LDA_INDY: {
            Byte ZeroPageAddress{FetchByte(NumOfCycles, Mbar)};
            Word ActualAddress{ReadWord(NumOfCycles, ZeroPageAddress, Mbar)};
            Word ActualAddressY = ActualAddress + Y;
            A = ReadByte(NumOfCycles, ActualAddressY, Mbar);
            if (ActualAddressY - ActualAddress >= 0xFF)
            {
                NumOfCycles--;
            }
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
                throw std::runtime_error(std::format("Illegal Instruction {:#04X} Detected!", Instruction));
            }
            break;
        }
    }
    const std::int32_t NumOfCyclesUsed(CyclesRequested - NumOfCycles);
    return NumOfCyclesUsed;
}

} // namespace MOS6502