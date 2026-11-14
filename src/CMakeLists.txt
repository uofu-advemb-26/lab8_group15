# This is the main binary. List your C files here.
add_executable(activity0_tx
    activity0_tx.c
)

pico_set_program_name(activity0_tx "test")
pico_set_program_version(activity0_tx "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(activity0_tx 1)
pico_enable_stdio_usb(activity0_tx 0)

# Add the libraries to the build
target_link_libraries(activity0_tx PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  can2040
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(activity0_tx PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(activity0_tx)

find_program(OPENOCD openocd)
find_program(PICOTOOL picotool)

add_custom_target(flash_tx ${PICOTOOL} load -f $<TARGET_FILE:activity0_tx> DEPENDS activity0_tx)
add_custom_target(openocd_flash_tx ${OPENOCD} -s ${OPENOCD_PATH} -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c 'adapter speed 5000' -c program "$<TARGET_FILE:activity0_tx>" verify reset exit' DEPENDS activity0_tx)





# This is the main binary. List your C files here.
add_executable(activity0_rx
    activity0_rx.c
)

pico_set_program_name(activity0_rx "test")
pico_set_program_version(activity0_rx "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(activity0_rx 1)
pico_enable_stdio_usb(activity0_rx 0)

# Add the libraries to the build
target_link_libraries(activity0_rx PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  can2040
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(activity0_rx PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(activity0_rx)

find_program(OPENOCD openocd)
find_program(PICOTOOL picotool)

add_custom_target(flash_rx ${PICOTOOL} load -f $<TARGET_FILE:activity0_rx> DEPENDS activity0_rx)
add_custom_target(openocd_flash_rx ${OPENOCD} -s ${OPENOCD_PATH} -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c 'adapter speed 5000' -c program "$<TARGET_FILE:activity0_rx>" verify reset exit' DEPENDS activity0_rx)



add_executable(activity1_high
    activity1_high.c
)

pico_set_program_name(activity1_high "test")
pico_set_program_version(activity1_high "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(activity1_high 1)
pico_enable_stdio_usb(activity1_high 0)

# Add the libraries to the build
target_link_libraries(activity1_high PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  can2040
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(activity1_high PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(activity1_high)

find_program(OPENOCD openocd)
find_program(PICOTOOL picotool)

add_custom_target(flash_high ${PICOTOOL} load -f $<TARGET_FILE:activity1_high> DEPENDS activity1_high)
add_custom_target(openocd_flash_high ${OPENOCD} -s ${OPENOCD_PATH} -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c 'adapter speed 5000' -c program "$<TARGET_FILE:activity1_high>" verify reset exit' DEPENDS activity1_high)




add_executable(activity1_low
    activity1_low.c
)

pico_set_program_name(activity1_low "test")
pico_set_program_version(activity1_low "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(activity1_low 1)
pico_enable_stdio_usb(activity1_low 0)

# Add the libraries to the build
target_link_libraries(activity1_low PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  can2040
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(activity1_low PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(activity1_low)

find_program(OPENOCD openocd)
find_program(PICOTOOL picotool)

add_custom_target(flash_low ${PICOTOOL} load -f $<TARGET_FILE:activity1_low> DEPENDS activity1_low)
add_custom_target(openocd_flash_low ${OPENOCD} -s ${OPENOCD_PATH} -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c 'adapter speed 5000' -c program "$<TARGET_FILE:activity1_low>" verify reset exit' DEPENDS activity1_low)
