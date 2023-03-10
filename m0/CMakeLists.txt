cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(include)

target_sources(app PRIVATE 
    src/usb_descriptors.c
    src/usbd_dfu.c
    src/flash.c
    src/console.c
    src/partition.c
    src/softcrc.c
    )

add_subdirectory(src/cmd)
add_subdirectory(../components/ipc ${build_dir}/ipc)
add_subdirectory(../components/rpmsg-lite ${build_dir}/rpmsg-lite)

target_compile_definitions(app PRIVATE -DUSBD_DFU_XFER_SIZE=1024)
target_compile_definitions(app PRIVATE -DUSBD_DFU_APP_DEFAULT_ADD=0x58000000)
target_compile_options(app PRIVATE -ggdb -O0)
# we need to set it on the shell library instead 
#target_compile_definitions(app PRIVATE -DSHELL_DEFAULT_NAME="0x64>")

sdk_set_main_file(src/main.c)
sdk_set_linker_script(bl808_flash_m0.ld)
project(ox64_coproc)
