set(CPU_IMPLEMENT_FILES CoreImpl.cpp MemoryImpl.cpp)

target_sources(${DISASSEMBLER_NAME}
    PRIVATE
        DisassemblerImpl.cpp
)

target_sources(${PROCESSOR_NAME}
    PRIVATE ${CPU_IMPLEMENT_FILES}
)