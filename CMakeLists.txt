cmake_minimum_required(VERSION 3.20)
project(FalloutTerminal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/audio_manager.cpp
    src/terminal_ui.cpp
    src/menu_builder.cpp
    src/application.cpp
    src/file_system_navigator.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::json
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
)

if(EXISTS ${CMAKE_SOURCE_DIR}/sounds)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/sounds
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/sounds
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/sounds
        COMMENT "Copying sounds directory"
        VERBATIM
    )
endif()

if(WIN32)
    install(TARGETS ${PROJECT_NAME}
        RUNTIME_DEPENDENCY_SET runtime_deps
    )

    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
        DESTINATION .
        FILES_MATCHING PATTERN "*.dll"
    )
endif()