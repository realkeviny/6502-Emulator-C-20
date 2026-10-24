set(EXECUTABLE_NAME MainEmu)

add_executable(${EXECUTABLE_NAME} "main_emu.cpp")

target_include_directories(MainEmu PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${PROCESSOR_NAME})

if(${ENABLE_LTO})
    target_enable_lto(${EXECUTABLE_NAME} ${ENABLE_LTO})
endif()