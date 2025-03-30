cmake_minimum_required(VERSION 3.20)
project(FalloutTerminal 
    LANGUAGES CXX 
    VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Boost REQUIRED COMPONENTS json system)

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
    Boost::system
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

install(TARGETS ${PROJECT_NAME}
    RUNTIME_DEPENDENCY_SET runtime_deps 
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/sounds/ DESTINATION sounds)
install(FILES ${CMAKE_SOURCE_DIR}/config.json DESTINATION .)  
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt DESTINATION .)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "YourCompany")
set(CPACK_PACKAGE_DESCRIPTION "Fallout-style terminal interface")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_DISPLAY_NAME "Fallout Terminal")
else()
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS 
        "libsdl2-2.0-0, libsdl2-mixer-2.0-0, libboost-json-dev"
    )
endif()

include(CPack)