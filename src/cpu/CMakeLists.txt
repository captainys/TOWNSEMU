add_library(cpu
cpu.h
cpu.cpp
i486.h
i486.cpp
i487.cpp
i486inst.h
i486instfetch.h
i486inst.cpp
i486operand.cpp
i486debug.h
i486debug.cpp
i486debugmemaccess.h
i486debugmemaccess.cpp
i486symtable.h
i486symtable.cpp
i486symtableexp.cpp
i486statesave.cpp
i486fidelity.h
)
target_link_libraries(cpu device ramrom inout cpputil)
target_include_directories(cpu PUBLIC .)
