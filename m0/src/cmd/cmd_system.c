#include <stdio.h>
#include <bflb_flash.h>
#include <bflb_efuse.h>
#include <bl808_ef_cfg.h>
#include <bl808_sflash.h>
#include <bl808_clock.h>
#include "cmd_system.h"

char *psram_to_string(uint8_t val) {
    switch (val) {
        case 0:
            return "None";
        case 1:
            return "WB 4MB";
        case 2:
            return "UHS 32MB";
        case 3:
            return "UHS 64MB";
        case 4:
            return "WB 32MB";
        case 5:
            return "WB 16MB";
        default:
            return "Unknown";
    }
}

int cmd_sysinfo(int argc, char **argv)
{
    if (argc != 1) {
        printf("Usage: system");
        return 0;
    }
    {
        bflb_efuse_device_info_type di;
        bflb_ef_ctrl_get_device_info(&di);
        printf("=========== Chip info ==============\r\n");
        printf("Chip Revision     0x%08X\r\n", di.chipInfo);
        printf("Integrated Flash: %s\r\n", di.memoryInfo == 0 ? "None" : "1MB");
        printf("PSRam:            %s\r\n", psram_to_string(di.psramInfo));
        printf("DeviceInfo        0x%08X\r\n", di.deviceInfo);

        uint8_t mac[6];
        bflb_efuse_read_mac_address_opt(0, &mac[0], 0);
        printf("Mac Address:      %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    }
    {
        SPI_Flash_Cfg_Type flashCfg;
        uint8_t *pFlashCfg = NULL;
        uint32_t flashCfgLen = 0;
        uint32_t flashJedecId = 0;

        flashJedecId = bflb_flash_get_jedec_id();
        bflb_flash_get_cfg(&pFlashCfg, &flashCfgLen);
        arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
        printf("=========== External Flash ==============\r\n");
        printf("jedec id   0x%06X\r\n", flashJedecId);
        printf("mid            0x%02X\r\n", flashCfg.mid);
        printf("iomode         0x%02X\r\n", flashCfg.ioMode);
        printf("clk delay      0x%02X\r\n", flashCfg.clkDelay);
        printf("clk invert     0x%02X\r\n", flashCfg.clkInvert);
        printf("read reg cmd0  0x%02X\r\n", flashCfg.readRegCmd[0]);
        printf("read reg cmd1  0x%02X\r\n", flashCfg.readRegCmd[1]);
        printf("write reg cmd0 0x%02X\r\n", flashCfg.writeRegCmd[0]);
        printf("write reg cmd1 0x%02X\r\n", flashCfg.writeRegCmd[1]);
        printf("qe write len   0x%02X\r\n", flashCfg.qeWriteRegLen);
        printf("cread support  0x%02X\r\n", flashCfg.cReadSupport);
        printf("cread code     0x%02X\r\n", flashCfg.cReadMode);
        printf("burst wrap cmd 0x%02X\r\n", flashCfg.burstWrapCmd);
    }
    {
        printf("=========== Clocks ===================\r\n");
        printf("System Core Clock: %dMhz\r\n", SystemCoreClockGet()/1000/1000);
        printf("System Clock MCU XCLK: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_XCLK)/1000/1000);
        printf("System Clock MCU Root Clock: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_ROOT_CLK)/1000/1000);
        printf("System Clock MCU Clock: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK)/1000/1000);
        printf("System Clock MCU PBCLK: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK)/1000/1000);
        printf("System Clock DSP XCLK: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_DSP_XCLK)/1000/1000);
        printf("System Clock DSP Root Clock: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_DSP_ROOT_CLK)/1000/1000);
        printf("System Clock DSP Clock: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_DSP_CLK)/1000/1000);
        printf("System Clock DSP BCLK: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_DSP_BCLK)/1000/1000);
        printf("System Clock DSP PBCLK: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_DSP_PBCLK)/1000/1000);
        printf("System Clock LP Clock: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_LP_CLK)/1000/1000);
        printf("System Clock F32K Clock: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_F32K)/1000/1000);
        printf("System Clock XTAL Clock: %dMhz\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XTAL)/1000/1000);
    }





    return 0;
}
