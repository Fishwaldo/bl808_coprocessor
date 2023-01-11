cmake_minimum_required(VERSION 3.15)

include(proj.conf)
include(../../cmake/top.cmake)

sdk_add_include_directories(include)

target_sources(app PRIVATE 
    )

#add_subdirectory(../common/ipc ${build_dir}/ipc)
#add_subdirectory(../common/rpmsg-lite ${build_dir}/rpmsg-lite)

target_compile_options(app PRIVATE -ggdb -O0)
# we need to set it on the shell library instead 
#target_compile_definitions(app PRIVATE -DSHELL_DEFAULT_NAME="0x64>")

sdk_set_main_file(src/main.c)
sdk_set_linker_script(bl808_flash_d0.ld)
project(ox64_coproc)
