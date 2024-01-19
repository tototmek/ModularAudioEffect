set(MODULE_NAME blocks)
set(LIBRARY_NAME "${MODULE_NAME}")

include_directories(processes/)

set(MODULE_SRC 
adder.cpp
block.cpp
block_system.cpp
evaluation_sequence.cpp
process_block.cpp
processes/delay.cpp
processes/gain.cpp
splitter.cpp
)

add_library(${LIBRARY_NAME} STATIC ${MODULE_SRC})
target_link_libraries(${LIBRARY_NAME} PRIVATE
spdlog::spdlog
)