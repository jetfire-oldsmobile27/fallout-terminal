cmake_minimum_required(VERSION 3.20)
project(FalloutTerminal
    LANGUAGES CXX 
    VERSION 1.76.2287
)

message(STATUS "
=============================================

██╗   ██╗ █████╗ ██╗   ██╗██╗     ████████╗  
██║   ██║██╔══██╗██║   ██║██║     ╚══██╔══╝  
██║   ██║███████║██║   ██║██║        ██║     
╚██╗ ██╔╝██╔══██║██║   ██║██║        ██║     
 ╚████╔╝ ██║  ██║╚██████╔╝███████╗   ██║     
  ╚═══╝  ╚═╝  ╚═╝ ╚═════╝ ╚══════╝   ╚═╝     
  
    ████████╗███████╗ ██████╗
    ╚══██╔══╝██╔════╝██╔════╝
       ██║   █████╗  ██║     
       ██║   ██╔══╝  ██║     
       ██║   ███████╗╚██████╗
         ╚═╝   ╚══════╝ ╚═════╝                                                                   
=============================================
")
message(STATUS "Vault-Tec Terminal System v${PROJECT_VERSION}")
message(STATUS "Better Safe Than Sorry!\n")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Boost 1.75 REQUIRED COMPONENTS json)

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

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)
set(ASSETS_DIR ${RESOURCES_DIR}/assets)
set(SOUNDS_DIR ${RESOURCES_DIR}/sounds)

foreach(file 
    "${ASSETS_DIR}/app.ico"
    "${RESOURCES_DIR}/config.json"
    "${RESOURCES_DIR}/LICENSE.txt"  
)
    if(NOT EXISTS ${file})
        message(FATAL_ERROR "Required file not found: ${file}")
    endif()
endforeach()

add_custom_command(TARGET VaultTerminal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sounds
    COMMAND ${CMAKE_COMMAND} -E copy
        ${ASSETS_DIR}/app.ico
        ${RESOURCES_DIR}/config.json
        ${RESOURCES_DIR}/LICENSE.txt  
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SOUNDS_DIR}/
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sounds
    COMMENT "Copying runtime resources..."
    VERBATIM
)

install(TARGETS VaultTerminal
    RUNTIME DESTINATION bin
)

install(FILES
    ${ASSETS_DIR}/app.ico
    ${RESOURCES_DIR}/config.json
    ${RESOURCES_DIR}/LICENSE.txt  
    DESTINATION bin
)

install(DIRECTORY
    ${SOUNDS_DIR}/
    DESTINATION bin/sounds
)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "VaultTerminal")
set(CPACK_PACKAGE_VENDOR "Vault-Tec Corporation")
set(CPACK_RESOURCE_FILE_LICENSE "${RESOURCES_DIR}/LICENSE.txt")  
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\app.ico")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_MODIFY_PATH ON)
    
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        CreateShortCut \\\"$DESKTOP\\\\Vault Terminal.lnk\\\" \\
            \\\"$INSTDIR\\\\bin\\\\VaultTerminal.exe\\\" \\
            \\\"\\\" \\
            \\\"$INSTDIR\\\\bin\\\\app.ico\\\"
    ")
endif()

include(CPack)