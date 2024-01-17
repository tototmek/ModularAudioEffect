cmake_minimum_required(VERSION 3.13.3)

project(ModularAudioEffect VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

configure_file(src/project_info.h.in project_info.h)
include_directories(${CMAKE_BINARY_DIR})

# add dependencies
include(cmake/fetch_dependencies.cmake)
find_package(spdlog REQUIRED)

# add submodules
add_subdirectory(src/blocks)
add_subdirectory(src/audio)
set(PROJECT_LIBS blocks audio)

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    spdlog::spdlog
    ${PROJECT_LIBS}
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