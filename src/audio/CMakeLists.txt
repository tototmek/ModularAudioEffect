set(MODULE_NAME audio)
set(LIBRARY_NAME "${MODULE_NAME}")

find_package(portaudio REQUIRED)

set(MODULE_SRC 
audio_client_portaudio.cpp
)

add_library(${LIBRARY_NAME} STATIC ${MODULE_SRC})
target_link_libraries(${LIBRARY_NAME} PRIVATE
spdlog::spdlog
portaudio
)