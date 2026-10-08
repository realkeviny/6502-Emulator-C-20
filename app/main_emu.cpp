import CPU_6502;

int main(int argc, char const *argv[])
{
    Memory bar1;
    CPU MyCPU;
    MyCPU.Reset(bar1);
    bar1[0xFFFC] = CPU::INST_JSR;
    bar1[0xFFFD] = 0x22;
    bar1[0xFFFE] = 0x33;
    bar1[0x3322] = CPU::INST_LDA_IMM;
    bar1[0x3323] = 0x17;

    MyCPU.Execute(8, bar1);
    return 0;
}