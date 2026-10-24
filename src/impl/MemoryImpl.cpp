module;

module cpu;

import std;

namespace MOS6502
{
void Memory::init()
{
    std::ranges::fill(Data, 0);
}

void Memory::WriteWord(Word Value, std::uint32_t Address, std::int32_t &NumOfCycles)
{
    Data[Address] = Value & 0xFF;
    Data[Address + 1] = (Value >> 8);
    NumOfCycles -= 2;
}
} // namespace MOS6502