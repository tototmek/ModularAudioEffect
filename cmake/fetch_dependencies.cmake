include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
)
include_directories(${spdlog_INCLUDE_DIRS})

FetchContent_Declare(
  PortAudio
  GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
  GIT_TAG        v19.7.0)
include_directories(${PortAudio_SOURCE_DIR}/include)

FetchContent_Declare(
  Catch2
  GIT_SHALLOW    TRUE
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

FetchContent_MakeAvailable(spdlog PortAudio Catch2)