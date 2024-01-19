find_package(Catch2 REQUIRED)

set(tests_SRC
test_blocks.cpp
test_graphs.cpp
)

add_executable(UnitTests ${tests_SRC})
target_link_libraries(UnitTests PRIVATE
    spdlog::spdlog
    Catch2::Catch2WithMain
    ${PROJECT_LIBS})