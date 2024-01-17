set(MODULE_NAME audio)
set(LIBRARY_NAME "${MODULE_NAME}")

set(MODULE_SRC 
main.cpp
)

add_library(${LIBRARY_NAME} STATIC ${MODULE_SRC})
target_link_libraries(${LIBRARY_NAME} PRIVATE
spdlog::spdlog
)