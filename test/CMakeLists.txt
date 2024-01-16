find_package(Catch2 3 REQUIRED)

set(tests_SRC
test_blocks.cpp
)

add_executable(UnitTests ${tests_SRC})
target_link_libraries(UnitTests PRIVATE ${PROJECT_LIBS} ${EXTERNAL_LIBS} Catch2::Catch2WithMain)