add_executable(disasm_addr disasm_addr.cpp)
target_link_libraries(disasm_addr towns)
add_test(NAME disasm_addr COMMAND disasm_addr)

add_executable(disasm_operand disasm_operand.cpp)
target_link_libraries(disasm_operand towns)
add_test(NAME disasm_operand COMMAND disasm_operand)
