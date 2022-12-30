sdk_generate_library()
sdk_library_add_sources(
    src/ipc.c
    src/bl808_ipc.c
)
sdk_add_include_directories(
    include
)
