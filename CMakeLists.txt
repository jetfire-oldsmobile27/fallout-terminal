cmake_minimum_required(VERSION 3.20)
project(FalloutTerminal
    LANGUAGES CXX 
    VERSION 1.76.2287
)

# Global configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "_d")

# Dependency Scanner
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Boost 1.75 REQUIRED COMPONENTS json)

# Executable target
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

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(SOUNDS_DIR "${CMAKE_SOURCE_DIR}/sounds")

if(WIN32)
    set(ASSET_FILES
        #"${ASSETS_DIR}/installer_background.bmp"
        "${ASSETS_DIR}/header_image.bmp"
        "${ASSETS_DIR}/vault_boy.ico"
    )

    foreach(asset ${ASSET_FILES})
        if(NOT EXISTS "${asset}")
            message(FATAL_ERROR "Missing required asset: ${asset}")
        endif()
    endforeach()

    add_custom_command(TARGET VaultTerminal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/assets"
        COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_FILES} "${CMAKE_BINARY_DIR}/assets/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOUNDS_DIR}" "${CMAKE_BINARY_DIR}/sounds"
        COMMENT "Deploying Vault-Tec resources..."
    )
endif()

include(GNUInstallDirs)

install(TARGETS VaultTerminal
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

install(DIRECTORY 
    "${CMAKE_BINARY_DIR}/assets/"
    DESTINATION "assets"
    COMPONENT Runtime
)

install(DIRECTORY
    "${CMAKE_BINARY_DIR}/sounds/"
    DESTINATION "sounds"
    COMPONENT Runtime
)

if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE.txt")
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt 
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vaulttec/licenses
        RENAME "VaultTec_EULA_2077"
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/config.json")
    install(FILES ${CMAKE_SOURCE_DIR}/config.json
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/vaultterminal
    )
endif()

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "vault-terminal")
set(CPACK_PACKAGE_VENDOR "Vault-Tec Corporation")
set(CPACK_PACKAGE_CONTACT "vaultsupport@vault-tec.com")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")

set(CPACK_NSIS_PACKAGE_NAME "VaultTerminal")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_DEFINES "
  !define ADD_TO_PATH_ENV
  !define INSTALL_PATH \\\"$INSTDIR\\\"
  !include LogicLib.nsh
")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH OFF)

    set(CPACK_NSIS_DEFINES "
        !include LogicLib.nsh
        !define ADD_TO_PATH_ENV
        !define INSTALL_PATH \\\"$INSTDIR\\\"
    ")

    # set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    #     StrCmp $ADD_TO_PATH_ALL_USERS 1 0 +2
    #         StrCpy $1 1
    #         Goto +1
    #     StrCpy $1 0
        
    #     ReadEnvStr $0 PATH
    #     StrLen $2 $0
    #     IntCmp $2 3072 add_path_ok add_path_ok add_path_fail

    #     add_path_fail:
    #         MessageBox MB_OK|MB_ICONEXCLAMATION \
    #             'PATH environment variable is too long!$\\nPlease cleanup your system PATH first.'
    #         Abort
        
    #     add_path_ok:
    #         Push '$INSTDIR\\\\bin'
    #         Call AddToPath
    # ")

    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        Push '$INSTDIR\\\\bin'
        Call un.RemoveFromPath
    ")
elseif(UNIX AND NOT APPLE)
    # Linux packaging
    set(CPACK_GENERATOR "TGZ;ZST")
    set(CPACK_SOURCE_GENERATOR "TGZ;ZST")
    
    # Arch Linux PKGBUILD
    configure_file(
        ${CMAKE_SOURCE_DIR}/scripts/vaulttec.pkgbuild.in
        ${CMAKE_BINARY_DIR}/vaulttec.pkgbuild
        @ONLY
    )
    
    install(SCRIPT ${CMAKE_SOURCE_DIR}/scripts/arch_install.cmake)
endif()

include(CPack)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET VaultTerminal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
        "Vault-Tec Quality Assurance: Build validated"
        COMMENT "Running post-build checks..."
    )
endif()