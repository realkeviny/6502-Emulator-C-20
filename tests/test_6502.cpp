#include "catch2/catch_test_macros.hpp"
import CPU_6502;
#include <cstdint>

class M6502Test
{
public:
    Memory bar;
    CPU MyCPU;

    M6502Test()
    {
        MyCPU.Reset(bar);
    }
};

TEST_CASE_METHOD(M6502Test, "LDAImmediateCanLoadAValueIntoTheARegister", "[LDA]")
{
    // Given:
    bar[0xFFFC] = CPU::INST_LDA_IMM;
    bar[0xFFFD] = 0x84;

    // When:
    CPU CPUCopy{MyCPU};
    std::int32_t CycleUsed{MyCPU.Execute(2, bar)};

    // Then:
    REQUIRE(MyCPU.A == 0x84);
    REQUIRE(CycleUsed == 2);
    REQUIRE_FALSE(MyCPU.Z);
    REQUIRE(MyCPU.N);
    REQUIRE(MyCPU.C == CPUCopy.C);
    REQUIRE(MyCPU.B == CPUCopy.B);
    REQUIRE(MyCPU.D == CPUCopy.D);
    REQUIRE(MyCPU.I == CPUCopy.I);
    REQUIRE(MyCPU.V == CPUCopy.V);
}

TEST_CASE_METHOD(M6502Test, "LDAZeroPageModeCanLoadAValueIntoTheARegister", "[LDAZP]")
{
    // Given:
    bar[0xFFFC] = CPU::INST_LDA_ZRP;
    bar[0xFFFD] = 0x33;
    bar[0x0033] = 0x17;

    // When:
    CPU CPUCopy{MyCPU};
    std::int32_t CycleUsed{MyCPU.Execute(3, bar)};

    // Then:
    REQUIRE(MyCPU.A == 0x17);
    REQUIRE(CycleUsed == 3);
    REQUIRE_FALSE(MyCPU.Z);
    REQUIRE_FALSE(MyCPU.N);
    REQUIRE(MyCPU.C == CPUCopy.C);
    REQUIRE(MyCPU.B == CPUCopy.B);
    REQUIRE(MyCPU.D == CPUCopy.D);
    REQUIRE(MyCPU.I == CPUCopy.I);
    REQUIRE(MyCPU.V == CPUCopy.V);
}

TEST_CASE_METHOD(M6502Test, "LDAZeroPageXCanLoadAValueIntoTheARegister", "[LDAZPX]")
{
    // Given:
    MyCPU.X = 5;
    bar[0xFFFC] = CPU::INST_LDA_ZRPX;
    bar[0xFFFD] = 0x33;
    bar[0x0038] = 0x17;

    // When:
    CPU CPUCopy{MyCPU};
    std::int32_t CycleUsed{MyCPU.Execute(4, bar)};

    // Then:
    REQUIRE(MyCPU.A == 0x17);
    REQUIRE(CycleUsed == 4);
    REQUIRE_FALSE(MyCPU.Z);
    REQUIRE_FALSE(MyCPU.N);
    REQUIRE(MyCPU.C == CPUCopy.C);
    REQUIRE(MyCPU.B == CPUCopy.B);
    REQUIRE(MyCPU.D == CPUCopy.D);
    REQUIRE(MyCPU.I == CPUCopy.I);
    REQUIRE(MyCPU.V == CPUCopy.V);
}

TEST_CASE_METHOD(M6502Test, "LDAZeroPageXCanLoadAValueIntoTheARegisterWhenWrapsAround", "[LDAZPXW]")
{
    // Given:
    MyCPU.X = 0xFF;
    bar[0xFFFC] = CPU::INST_LDA_ZRPX;
    bar[0xFFFD] = 0x80;
    bar[0x007F] = 0x17;

    // When:
    CPU CPUCopy{MyCPU};
    std::int32_t CycleUsed{MyCPU.Execute(4, bar)};

    // Then:
    REQUIRE(MyCPU.A == 0x17);
    REQUIRE(CycleUsed == 4);
    REQUIRE_FALSE(MyCPU.Z);
    REQUIRE_FALSE(MyCPU.N);
    REQUIRE(MyCPU.C == CPUCopy.C);
    REQUIRE(MyCPU.B == CPUCopy.B);
    REQUIRE(MyCPU.D == CPUCopy.D);
    REQUIRE(MyCPU.I == CPUCopy.I);
    REQUIRE(MyCPU.V == CPUCopy.V);
}