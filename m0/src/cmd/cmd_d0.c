#include <stdio.h>
#include <bl808_glb.h>
#include "cmd_d0.h"


int d0_start(int argc, char *argv[])
{
    BL_Err_Type ret = GLB_Release_CPU(GLB_CORE_ID_D0);
    if (ret != SUCCESS) {
        printf("d0 start failed\r\n");
    } else {
        printf("d0 start success\r\n");
    }
    return 1;
}

int d0_stop(int argc, char *argv[])
{
    BL_Err_Type ret = GLB_Halt_CPU(GLB_CORE_ID_D0);
    if (ret != SUCCESS) {
        printf("d0 stop failed\r\n");
    } else {
        printf("d0 stop success\r\n");
    }
    return 1;
}

int d0_reset(int argc, char *argv[])
{
    /* GLB_DSP_SW_SYSTEM_CTRL_SYS */

    BL_Err_Type ret = GLB_DSP_SW_System_Reset(GLB_DSP_SW_SYSTEM_CTRL_SYS);
    if (ret != SUCCESS) {
        printf("d0 reset failed\r\n");
    } else {
        printf("d0 reset success\r\n");
    }
    return 1;
}

int d0_status(int argc, char *argv[])
{
    bool isHalt = BL_IS_REG_BIT_SET(BL_RD_REG(MM_GLB_BASE, MM_GLB_MM_SW_SYS_RESET), MM_GLB_REG_CTRL_MMCPU0_RESET);
    printf("d0: %s\r\n", isHalt ? "halt" : "running");
    return 1;
}


int cmd_d0(int argc, char *argv[])
{
    if (argc == 1) {
        printf("Missing Command\r\n");
        printf("d0 stop\t - Stop CPU\r\n");
        printf("do start\t - Start CPU\r\n");
        printf("d0 reset\t - Reset CPU\r\n");
        printf("d0 status\t - Status of D0 CPU\r\n");
        return 1;
    }
    if (strcasecmp(argv[1], "help") == 0) {
        printf("d0 help\r\n");
        return 0;
    }
    if (strcasecmp(argv[1], "start") == 0) {
        return d0_start(argc, argv);
    }
    if (strcasecmp(argv[1], "stop") == 0) {
        return d0_stop(argc, argv);
    }
    if (strcasecmp(argv[1], "reset") == 0) {
        return d0_reset(argc, argv);
    }
    if (strcasecmp(argv[1], "status") == 0) {
        return d0_status(argc, argv);
    }
    printf("d0: Unknown command\r\n");
    return 1;
}