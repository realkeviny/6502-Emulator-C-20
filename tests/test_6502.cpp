import CPU_6502;
import <cstdint>;
#include "catch2/catch_test_macros.hpp"

struct ExpectedState
{
    Byte A{};
    Byte X{}, Y{};
    Byte Z{}, N{};
    std::int32_t Cycles{};
};

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

    M6502Test &AddX(Byte X)
    {
        MyCPU.X = X;
        return *this;
    }
    M6502Test &Write(Word Address, Byte Value)
    {
        bar[Address] = Value;
        return *this;
    }

    // Add Program in
    M6502Test &LoadProgram(std::initializer_list<Byte> OpCodes)
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
        REQUIRE(ActualCycles == ES.Cycles);
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

TEST_CASE("Undefined Instruction without going into infinite loop", "[UIWGIL]")
{
    M6502Test{}.LoadProgram({0x00, 0x00}).VerifyWholeState(1, {.Cycles = 1});
}

TEST_CASE("LDA Immediate Mode", "[LDAIM]")
{
    M6502Test{}.LoadProgram({CPU::INST_LDA_IMM, 0x84}).VerifyWholeState(2, {.A = 0x84, .Z = 0, .N = 1, .Cycles = 2});
}

TEST_CASE("LDA Zero Page Mode", "[LDAZP]")
{
    M6502Test{}
        .Write(0x33, 0x17)
        .LoadProgram({CPU::INST_LDA_ZRP, 0x33})
        .VerifyWholeState(3, {.A = 0x17, .Z = 0, .N = 0, .Cycles = 3});
}

TEST_CASE("LDA Zero Page X", "[LDAZPX]")
{
    M6502Test{}
        .AddX(5)
        .Write(0x0038, 0x17)
        .LoadProgram({CPU::INST_LDA_ZRPX, 0x33})
        .VerifyWholeState(4, {.A = 0x17, .X = 5, .Z = 0, .N = 0, .Cycles = 4});
}

TEST_CASE("LDA Zero Page X Wrapping Around", "[LDAZPXW]")
{
    M6502Test{}
        .AddX(0xFF)
        .Write(0x007F, 0x17)
        .LoadProgram({CPU::INST_LDA_ZRPX, 0x80})
        .VerifyWholeState(4, {.A = 0x17, .X = 0xFF, .Z = 0, .N = 0, .Cycles = 4});
}