cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(include)

target_sources(app PRIVATE 
    )

add_subdirectory(../components/ipc ${build_dir}/ipc)
add_subdirectory(../components/rpmsg-lite ${build_dir}/rpmsg-lite)

target_compile_options(app PRIVATE -ggdb -O0)
# we need to set it on the shell library instead 
#target_compile_definitions(app PRIVATE -DSHELL_DEFAULT_NAME="0x64>")

sdk_set_main_file(src/main.c)
sdk_set_linker_script(bl808_flash_d0.ld)
project(ox64_coproc)
