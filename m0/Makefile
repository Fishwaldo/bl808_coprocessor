SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= /home/fish/bl808/bl_mcu_sdk

export BL_SDK_BASE

CHIP ?= bl808
BOARD ?= bl808dk
CPU_ID ?= m0
CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

include $(BL_SDK_BASE)/project.build

flash:
	bflb-mcu-tool --chipname=bl808 --firmware build/build_out/ox64_coproc_bl808_m0.bin

monitor:
	screen /dev/ttyACM0 2000000