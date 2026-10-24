import cpu;

int main(int argc, char const *argv[])
{
    MOS6502::Memory bar1;
    MOS6502::CPU MyCPU;
    MyCPU.Reset(bar1);
    bar1[0xFFFC] = MOS6502::CPU::INST_JSR;
    bar1[0xFFFD] = 0x22;
    bar1[0xFFFE] = 0x33;
    bar1[0x3322] = MOS6502::CPU::INST_LDA_IMM;
    bar1[0x3323] = 0x17;

    MyCPU.Execute(8, bar1);
    return 0;
}