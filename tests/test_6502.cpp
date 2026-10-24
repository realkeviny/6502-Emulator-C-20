import cpu;
import disassembler;
import <optional>;
#include "catch2/catch_test_macros.hpp"

using MOS6502::Byte;
using MOS6502::CPU;
using MOS6502::Memory;
using MOS6502::Word;

struct ExpectedState
{
    Byte A{};
    Byte X{}, Y{};
    Byte Z{}, N{};
    std::optional<std::int32_t> Cycles{std::nullopt};
};

/**
 * @class M6502Test
 * @brief Fluent test harness for MOS 6502 execution and timing verification.
 *
 * @par Generic Test Chaining Pattern:
 * @code
 * TEST_CASE("<INSTRUCTION_NAME> <SCENARIO_DESCRIPTION>", "[<TAG>]")
 * {
 *     M6502Test{}
 *         // 1. [Pre-Conditions] Configure initial CPU registers (optional)
 *         //    e.g., .SetX(val), .SetY(val), .SetStatus(val)
 *         [.<REGISTER_MODIFIER>(...)]
 *
 *         // 2. [Data Space] Pre-populate target memory operands/pointers (optional)
 *         //    Prepares data read by instructions, ZeroPage pointers, or vector tables.
 *         [.<Write>(<TARGET_ADDRESS>, <TEST_VALUE>)]...
 *
 *         // 3. [Code Stream] Stream executable machine code into PC (starts at 0xFFFC)
 *         //    Accepts arbitrary byte sequences: Opcode + Operands (0-2 bytes)
 *         //    or contiguous multi-instruction blocks.
 *         .LoadProgram({<OPCODE> [, <OPERAND_BYTES>...]})
 *
 *         // 4. [Execution & Invariants] Step cycles, assert mutations, verify untouched state
 *         //    Executes requested cycles, asserts ExpectedState mutations, and automatically
 *         //    validates that undisturbed flags (C, B, D, I, V) remain identical to baseline.
 *         .VerifyWholeState(<REQUESTED_CYCLES>, {
 *             .A = <EXPECTED_A>,
 *             .X = <EXPECTED_X>,
 *             .Y = <EXPECTED_Y>,
 *             .Z = <EXPECTED_Z>,
 *             .N = <EXPECTED_N>,
 *             .Cycles = <EXPECTED_CYCLES>
 *         });
 * }
 * @endcode
 */
class M6502Test
{
    Memory bar;
    CPU MyCPU;
    CPU ModelCPU;
    Word PC{0xFFFC};

public:
    M6502Test()
    {
        MyCPU.Reset(bar);
        ModelCPU = MyCPU;
    }

    M6502Test &SetX(MOS6502::Byte X)
    {
        MyCPU.X = X;
        return *this;
    }

    M6502Test &SetY(MOS6502::Byte Y)
    {
        MyCPU.Y = Y;
        return *this;
    }

    M6502Test &SetA(MOS6502::Byte A)
    {
        MyCPU.A = A;
        return *this;
    }

    M6502Test &Write(MOS6502::Word Address, MOS6502::Byte Value)
    {
        bar[Address] = Value;
        return *this;
    }

    // Add Program in
    M6502Test &LoadProgram(std::initializer_list<MOS6502::Byte> OpCodes)
    {
        for (auto code : OpCodes)
        {
            bar[PC++] = code;
        }
        return *this;
    }

    void VerifyWholeState(std::int32_t RequestedCycles, ExpectedState ES)
    {
        std::int32_t ActualCycles{MyCPU.Execute(RequestedCycles, bar)};
        if (ES.Cycles.has_value())
        {
            REQUIRE(ActualCycles == ES.Cycles.value());
        }
        REQUIRE(MyCPU.A == ES.A);
        REQUIRE(MyCPU.X == ES.X);
        REQUIRE(MyCPU.Y == ES.Y);
        REQUIRE(MyCPU.Z == ES.Z);
        REQUIRE(MyCPU.N == ES.N);

        REQUIRE(MyCPU.C == ModelCPU.C);
        REQUIRE(MyCPU.B == ModelCPU.B);
        REQUIRE(MyCPU.D == ModelCPU.D);
        REQUIRE(MyCPU.I == ModelCPU.I);
        REQUIRE(MyCPU.V == ModelCPU.V);
    }

    void VerifyCycleCountOnly(std::int32_t RequestedCycles, std::int32_t ExpectedCycles)
    {
        std::int32_t ActualCycles{MyCPU.Execute(RequestedCycles, bar)};
        REQUIRE(ActualCycles == ExpectedCycles);
    }
};

TEST_CASE("CPU Idle Test", "[IDLE]")
{
    M6502Test{}.VerifyWholeState(0, {.Cycles = 0});
}

TEST_CASE("More Cycles Than Requested If Required By Instruction", "[MCTR]")
{
    M6502Test{}.LoadProgram({CPU::INST_LDA_IMM, 0x84}).VerifyCycleCountOnly(1, 2);
}

TEST_CASE("LDA Immediate Mode", "[LDAIM]")
{
    M6502Test{}.LoadProgram({CPU::INST_LDA_IMM, 0x84}).VerifyWholeState(2, {.A = 0x84, .Z = 0, .N = 1, .Cycles = 2});
}

TEST_CASE("LDA Immediate Mode Affecting The Zero Flag", "[LDAIMAZF]")
{
    M6502Test{}.SetA(0x96).LoadProgram({CPU::INST_LDA_IMM, 0x0}).VerifyWholeState(2, {.A = 0x0, .Z = 1, .N = 0});
}

TEST_CASE("LDA Zero Page Mode", "[LDAZP]")
{
    M6502Test{}
        .LoadProgram({CPU::INST_LDA_ZRP, 0x33})
        .Write(0x33, 0x17)
        .VerifyWholeState(3, {.A = 0x17, .Z = 0, .N = 0, .Cycles = 3});
}

TEST_CASE("LDA Zero Page X", "[LDAZPX]")
{
    M6502Test{}
        .SetX(5)
        .LoadProgram({CPU::INST_LDA_ZRPX, 0x33})
        .Write(0x0038, 0x17)
        .VerifyWholeState(4, {.A = 0x17, .X = 5, .Z = 0, .N = 0, .Cycles = 4});
}

TEST_CASE("LDA Zero Page X Wrapping Around", "[LDAZPXW]")
{
    M6502Test{}
        .SetX(0xFF)
        .LoadProgram({CPU::INST_LDA_ZRPX, 0x80})
        .Write(0x007F, 0x17)
        .VerifyWholeState(4, {.A = 0x17, .X = 0xFF, .Z = 0, .N = 0, .Cycles = 4});
}

TEST_CASE("LDA Absolute", "[LDAABS]")
{
    M6502Test{}
        .LoadProgram({CPU::INST_LDA_ABS, 0x80, 0x44})
        .Write(0x4480, 0x17)
        .VerifyWholeState(4, {.A = 0x17, .Z = 0, .N = 0, .Cycles = 4});
}

TEST_CASE("LDA Absolute X", "[LDAABSX]")
{
    M6502Test{}
        .SetX(1)
        .LoadProgram({CPU::INST_LDA_ABSX, 0x80, 0x44})
        .Write(0x4481, 0x17)
        .VerifyWholeState(4, {.A = 0x17, .X = 1, .Z = 0, .N = 0, .Cycles = 4});
}

TEST_CASE("LDA Absolute X Page Boundary Crossing", "[LDAABSXPC]")
{
    M6502Test{}
        .SetX(0xFF)
        .LoadProgram({CPU::INST_LDA_ABSX, 0x02, 0x44}) // 0x4402
        .Write(0x4501, 0x17)                           // 0x4402 + 0xFF = 0x4501
        .VerifyWholeState(5, {.A = 0x17, .X = 0xFF, .Z = 0, .N = 0, .Cycles = 5});
}

TEST_CASE("LDA Absolute Y", "[LDAABSY]")
{
    M6502Test{}
        .SetY(1)
        .LoadProgram({CPU::INST_LDA_ABSY, 0x80, 0x44})
        .Write(0x4481, 0x17)
        .VerifyWholeState(4, {.A = 0x17, .Y = 1, .Z = 0, .N = 0, .Cycles = 4});
}

TEST_CASE("LDA Absolute Y Page Boundary Crossing", "[LDAABSYPBC]")
{
    M6502Test{}
        .SetY(0xFF)
        .LoadProgram({CPU::INST_LDA_ABSY, 0x02, 0x44}) // 0x4402
        .Write(0x4501, 0x17)                           // 0x4402 + 0xFF = 0x4501
        .VerifyWholeState(5, {.A = 0x17, .Y = 0xFF, .Z = 0, .N = 0, .Cycles = 5});
}

TEST_CASE("LDA Indexed Indirect X", "[LDAINDX]")
{
    M6502Test{}
        .SetX(0x04)
        .LoadProgram({CPU::INST_LDA_INDX, 0x02})
        .Write(0x0006, 0x00)
        .Write(0x0007, 0x80)
        .Write(0x8000, 0x41)
        .VerifyWholeState(6, {.A = 0x41, .X = 0x04, .Z = 0, .N = 0, .Cycles = 6});
}

TEST_CASE("LDA Indirect Indexed Y", "[LDAINDY]")
{
    M6502Test{}
        .SetY(0x04)
        .LoadProgram({CPU::INST_LDA_INDY, 0x02})
        .Write(0x0002, 0x00)
        .Write(0x0003, 0x80)
        .Write(0x8004, 0x41) // 0x8000 + 0x04
        .VerifyWholeState(5, {.A = 0x41, .Y = 0x04, .Z = 0, .N = 0, .Cycles = 5});
}

TEST_CASE("LDA Indirect Indexed Y Page Boundary Crossing", "[LDAINDYPBC]")
{
    M6502Test{}
        .SetY(0xFF)
        .LoadProgram({CPU::INST_LDA_INDY, 0x02})
        .Write(0x0002, 0x02)
        .Write(0x0003, 0x80)
        .Write(0x8101, 0x41) // 0x8002 + 0xFF
        .VerifyWholeState(6, {.A = 0x41, .Y = 0xFF, .Z = 0, .N = 0, .Cycles = 6});
}