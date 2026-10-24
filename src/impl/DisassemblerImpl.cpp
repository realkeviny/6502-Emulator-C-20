
module;

module disassembler;

auto InstructionDisassembler(std::span<const std::uint8_t> Buffer, std::uint16_t PC) -> std::expected<Result, Errors>
{
    if (Buffer.empty())
    {
        return std::unexpected(Errors::BufferTooShort);
    }

    const std::uint8_t Operation{Buffer[0]};
    const auto &MetaCode{OPCODE_TABLE[Operation]};

    if (MetaCode.CurrentMode == AddressMode::Unknown)
    {
        return std::unexpected(Errors::UnknownOpcode);
    }

    if (Buffer.size() < MetaCode.Length)
    {
        return std::unexpected(Errors::BufferTooShort);
    }

    std::string Operand;
    switch (MetaCode.CurrentMode)
    {
    case AddressMode::Immediate: {
        Operand = std::format("#${:02X}", Buffer[1]);
        break;
    }
    case AddressMode::ZeroPage: {
        Operand = std::format("${:02X}", Buffer[1]);
        break;
    }
    case AddressMode::ZeroPageX: {
        Operand = std::format("${:02X},X", Buffer[1]);
        break;
    }
    case AddressMode::Absolute: {
        std::uint16_t TargetAddress{static_cast<std::uint16_t>(Buffer[1] | Buffer[2] << 8)};
        Operand = std::format("${:04X}", TargetAddress);
        break;
    }
    default:
        break;
    }

    return Result{.Address = PC, .Length = MetaCode.Length, .Text = std::format("{} {}", MetaCode.Mnemonic, Operand)};
}