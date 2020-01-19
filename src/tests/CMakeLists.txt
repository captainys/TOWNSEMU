add_executable(disasm_addr disasm_addr.cpp)
target_link_libraries(disasm_addr towns)

add_executable(disasm_operand disasm_operand.cpp)
target_link_libraries(disasm_operand towns)
