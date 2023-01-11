sdk_generate_library()
sdk_library_add_sources(
    src/ipc.c
    src/ipc_irq.c
)
sdk_add_include_directories(
    include
)
