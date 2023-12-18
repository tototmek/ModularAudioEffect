set(MODULE_NAME blocks)
set(LIBRARY_NAME "${MODULE_NAME}")

include_directories(processes/)

set(MODULE_SRC 
processes/gain.cpp
processes/delay.cpp
single_process_block.cpp
)

add_library(${LIBRARY_NAME} STATIC ${MODULE_SRC})