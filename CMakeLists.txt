cmake_minimum_required(VERSION 3.13.3)

project(ModularAudioEffect VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

configure_file(src/project_info.h.in project_info.h)
include_directories(${CMAKE_BINARY_DIR})

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# add dependencies
if (NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()
set(EXTERNAL_LIBS ${EXTERNAL_LIBS} spdlog::spdlog)

# add submodules
add_subdirectory(src/blocks)
set(PROJECT_LIBS ${PROJECT_LIBS} blocks)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${PROJECT_LIBS}
    ${EXTERNAL_LIBS}
)

SET(ENABLE_TESTING TRUE CACHE BOOL "Enable testing")
if (${ENABLE_TESTING}) 
# add tests
    add_subdirectory(test)
    enable_testing()
    add_test(
        NAME UnitTests
        COMMAND $<TARGET_FILE:UnitTests>
    )
endif()