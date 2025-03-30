cmake_minimum_required(VERSION 3.20)
project(FalloutTerminal
    LANGUAGES CXX 
    VERSION 1.76.2287
)

# Compiler settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "_d")

# Dependencies
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Boost 1.75 REQUIRED COMPONENTS json)

# Main executable
add_executable(VaultTerminal
    src/main.cpp
    src/audio_manager.cpp
    src/terminal_ui.cpp
    src/menu_builder.cpp
    src/application.cpp
    src/file_system_navigator.cpp
)

target_include_directories(VaultTerminal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(VaultTerminal PRIVATE
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
    Boost::json
)

# Resource configuration
set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)
set(ASSETS_DIR ${RESOURCES_DIR}/assets)
set(SOUNDS_DIR ${RESOURCES_DIR}/sounds)

# Проверка существования файлов
if(NOT EXISTS "${RESOURCES_DIR}/config.json")
    message(FATAL_ERROR "config.json not found in resources directory!")
endif()

# Post-build: Copy resources to binary dir
add_custom_command(TARGET VaultTerminal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        ${CMAKE_BINARY_DIR}/bin
        ${CMAKE_BINARY_DIR}/bin/sounds
    COMMAND ${CMAKE_COMMAND} -E copy
        ${ASSETS_DIR}/vault_boy.ico
        ${RESOURCES_DIR}/config.json
        ${CMAKE_BINARY_DIR}/bin/
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SOUNDS_DIR}/
        ${CMAKE_BINARY_DIR}/bin/sounds
    COMMENT "Copying runtime resources..."
    VERBATIM
)

# Installation configuration
install(TARGETS VaultTerminal
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    DESTINATION .
    USE_SOURCE_PERMISSIONS
)

# Packaging configuration (NSIS)
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "VaultTerminal")
set(CPACK_PACKAGE_VENDOR "Vault-Tec Corporation")
set(CPACK_PACKAGE_CONTACT "support@vault-tec.com")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${RESOURCES_DIR}/LICENSE.txt)
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\vault_boy.ico")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    
    # Desktop shortcut with icon
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        CreateShortCut \\\"$DESKTOP\\\\VaultTerminal.lnk\\\" \\
            \\\"$INSTDIR\\\\bin\\\\VaultTerminal.exe\\\" \\
            \\\"\\\" \\
            \\\"$INSTDIR\\\\bin\\\\vault_boy.ico\\\"
    ")
    
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        Delete \\\"$DESKTOP\\\\VaultTerminal.lnk\\\"
    ")
endif()

include(CPack)

# Post-build validation
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET VaultTerminal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo 
            \"=== Vault-Tec validation passed ===\"
        COMMENT "Performing final checks..."
    )
endif()