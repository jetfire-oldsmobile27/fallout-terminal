cmake_minimum_required(VERSION 3.20)
project(FalloutTerminal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_definitions(-DWINDOWS)
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)
find_package(SDL2 REQUIRED)
find_library(SDL2_MIXER_LIBRARY NAMES SDL2_mixer)

add_executable(FalloutTerminal
    src/main.cpp
    src/audio_manager.cpp
    src/terminal_ui.cpp
    src/menu_builder.cpp
    src/application.cpp
    src/file_system_navigator.cpp
)

target_include_directories(FalloutTerminal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(FalloutTerminal PRIVATE
    Boost::json
    SDL2::SDL2
    ${SDL2_MIXER_LIBRARY}
)

add_custom_command(TARGET FalloutTerminal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:FalloutTerminal>/sounds"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/sounds"
        "$<TARGET_FILE_DIR:FalloutTerminal>/sounds"
    COMMENT "Copying sounds directory to build folder"
    VERBATIM
)