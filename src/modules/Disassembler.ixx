module;

export module disassembler;

import std;

export {
    enum class AddressMode : std::uint8_t
    {
        Unknown,
        Immediate,
        ZeroPage,
        ZeroPageX,
        Absolute
    };

    struct OpCodeMeta
    {
        std::string_view Mnemonic{"???"};
        AddressMode CurrentMode{AddressMode::Unknown};
        std::uint8_t Length{1};
    };

    struct Result
    {
        std::uint16_t Address;
        std::uint8_t Length;
        std::string Text;
    };

    enum class Errors
    {
        BufferTooShort,
        UnknownOpcode
    };

    consteval auto OpcodeTableCreation()
    {
        std::array<OpCodeMeta, 256> CodeTable{};

        CodeTable[0xA9] = {"LDA", AddressMode::Immediate, 2};
        CodeTable[0xA5] = {"LDA", AddressMode::ZeroPage, 2};
        CodeTable[0xB5] = {"LDA", AddressMode::ZeroPageX, 2};
        CodeTable[0x20] = {"JSR", AddressMode::Absolute, 3};

        return CodeTable;
    }

    inline constexpr auto OPCODE_TABLE{OpcodeTableCreation()};

    auto InstructionDisassembler(std::span<const std::uint8_t> Buffer, std::uint16_t PC)
        -> std::expected<Result, Errors>;
}