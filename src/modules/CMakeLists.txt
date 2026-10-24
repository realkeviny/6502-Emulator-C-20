set(MODULE_FOR_C85 Types.ixx Memory.ixx Core.ixx CPU.ixx )
set(DISASSEMBLING_MODULE_FOR_C85 Disassembler.ixx)
add_library(${PROCESSOR_NAME} STATIC)
add_library(${DISASSEMBLER_NAME} STATIC)

target_sources(${PROCESSOR_NAME}
    PUBLIC
        FILE_SET CXX_MODULES TYPE CXX_MODULES FILES
            ${MODULE_FOR_C85}
)
target_sources(${DISASSEMBLER_NAME}
    PUBLIC
        FILE_SET CXX_MODULES TYPE CXX_MODULES FILES
            ${DISASSEMBLING_MODULE_FOR_C85}
)


if(${ENABLE_LTO})
    target_enable_lto(${PROCESSOR_NAME} ${ENABLE_LTO})
endif()