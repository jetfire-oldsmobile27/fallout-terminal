message(STATUS "Vault-Tec Authentication System")
execute_process(
    COMMAND bash -c "read -p 'Do you accept the Vault-Tec EULA? [Y/N]: ' ans; echo $ans"
    OUTPUT_VARIABLE response
)

if(NOT response MATCHES "[Yy]")
    message(FATAL_ERROR "Access denied. Vault door remains sealed.")
endif()

execute_process(
    COMMAND sudo pacman -Sy --noconfirm sdl2 sdl2_mixer boost
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "ERROR: Super Mutant detected in dependency tunnels!")
endif()