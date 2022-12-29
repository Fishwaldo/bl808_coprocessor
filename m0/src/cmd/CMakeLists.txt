sdk_generate_library()
sdk_library_add_sources(
    cmd_memory.c 
    cmd_sensors.c
    cmd_system.c
    cmd_d0.c
    cmd_ipc.c
)
sdk_add_include_directories(.)

