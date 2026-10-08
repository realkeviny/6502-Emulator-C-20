set(EXECUTABLE_NAME MainEmu)

add_executable(${EXECUTABLE_NAME} "main_emu.cpp")

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})