set(MODULE_NAME blocks)
set(LIBRARY_NAME "${MODULE_NAME}")

set(MODULE_SRC 
gain.cpp
)

add_library(${LIBRARY_NAME} STATIC ${MODULE_SRC})