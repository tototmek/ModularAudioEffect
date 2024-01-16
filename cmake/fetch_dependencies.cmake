include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_SHALLOW    TRUE
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

FetchContent_MakeAvailable(spdlog Catch2)