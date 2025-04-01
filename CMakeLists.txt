cmake_minimum_required(VERSION 3.20)
project(VaultTerminal
    LANGUAGES CXX 
    VERSION 1.76.2287
)

# ==================== Initial Setup ====================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==================== Custom Messages ====================
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

# ==================== Dependencies ====================
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)

# ==================== Key Generation ====================
set(KEY_DIR ${CMAKE_BINARY_DIR}/keys)
set(PRIV_KEY ${KEY_DIR}/private_key.pem)
set(PUB_KEY ${KEY_DIR}/public_key.pem)

add_custom_command(
    OUTPUT ${PUB_KEY} ${PRIV_KEY}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${KEY_DIR}
    COMMAND openssl genpkey -algorithm RSA-PSS -pkeyopt rsa_keygen_bits:3072 
            -out ${PRIV_KEY}
    COMMAND openssl rsa -pubout -in ${PRIV_KEY} -out ${PUB_KEY}
    COMMENT "Generating RSA-PSS 3072-bit keys"
)

# ==================== Main Target ====================
add_executable(VaultTerminal
    src/main.cpp
    src/audio_manager.cpp
    src/terminal_ui.cpp
    src/menu_builder.cpp
    src/application.cpp
    src/file_system_navigator.cpp
    src/verifier.hpp
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
    OpenSSL::Crypto
)

# ==================== Post-Build Steps ====================
# Resource copying
set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)
set(ASSETS_DIR ${RESOURCES_DIR}/assets)
set(SOUNDS_DIR ${RESOURCES_DIR}/sounds)

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
)

# Platform-specific signing
if(UNIX)
    # Linux/macOS: Embed signature section
    target_link_options(VaultTerminal PRIVATE 
        "-Wl,--section-start=.signature=0x40000000"
    )

    add_custom_command(TARGET VaultTerminal POST_BUILD
        COMMAND openssl dgst -sha512 -sign ${PRIV_KEY} 
               -out ${KEY_DIR}/signature.bin $<TARGET_FILE:VaultTerminal>
        COMMAND objcopy --add-section .signature=${KEY_DIR}/signature.bin 
               $<TARGET_FILE:VaultTerminal>
        COMMAND xxd -i ${PUB_KEY} > ${CMAKE_SOURCE_DIR}/include/public_key.hpp
        COMMENT "Signing binary and embedding security artifacts"
        DEPENDS ${PRIV_KEY}
    )

elseif(WIN32)
    # # Windows: Use signtool (requires Windows SDK)
    # find_program(SIGNTOOL signtool HINTS "C:/Program Files (x86)/Windows Kits/10/bin/*/x64")
    # if(SIGNTOOL)
    #     add_custom_command(TARGET VaultTerminal POST_BUILD
    #         COMMAND ${SIGNTOOL} sign /fd SHA512 /f ${PRIV_KEY} 
    #                $<TARGET_FILE:VaultTerminal>
    #         COMMENT "Digitally signing binary (Windows)"
    #         DEPENDS ${PRIV_KEY}
    #     )
    # else()
    #     message(WARNING "SignTool not found - binary will not be signed!")
    # endif()

    # # Generate public key header for Windows
    # add_custom_command(TARGET VaultTerminal POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy ${PUB_KEY} 
    #            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/public.pem
    #     COMMAND xxd -i ${PUB_KEY} > ${CMAKE_SOURCE_DIR}/include/public_key.hpp
    #     COMMENT "Embedding public key"
    #     DEPENDS ${PUB_KEY}
    # )
endif()

# ==================== Installation ====================
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

# ==================== CPack Configuration ====================
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

# ==================== Final Checks ====================
# Verify critical resource files
foreach(file 
    "${ASSETS_DIR}/app.ico"
    "${RESOURCES_DIR}/config.json"
    "${RESOURCES_DIR}/LICENSE.txt"  
)
    if(NOT EXISTS ${file})
        message(FATAL_ERROR "Critical resource missing: ${file}")
    endif()
endforeach()