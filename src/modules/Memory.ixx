module;

export module cpu:memory;

import std;
import :types;

export namespace MOS6502
{
struct Memory
{
    static constexpr std::uint32_t MAX_SIZE{1024 * 64};
    std::array<Byte, MAX_SIZE> Data{};

    void init();

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

    void WriteWord(Word Value, std::uint32_t Address, std::int32_t &NumOfCycle);
};
} // namespace MOS6502