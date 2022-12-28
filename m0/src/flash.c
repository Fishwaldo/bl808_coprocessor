
#include "bflb_flash.h"
#include "usbd_dfu.h"
#include "log.h"

// uint8_t *dfu_read_flash(uint8_t *src, uint8_t *dest, uint32_t len)
// {
//     LOG_I("DFU Read %lx %lx %ld\r\n", src, dest, len);
//     uint32_t i = 0;
//     uint8_t *psrc = src;

//     for (i = 0; i < len; i++) {
//         dest[i] = *psrc++;
//     }
//     /* Return a valid address to avoid HardFault */
//     return (uint8_t *)(dest);
// }

// uint16_t dfu_write_flash(uint8_t *src, uint8_t *dest, uint32_t len)
// {
//     LOG_I("DFU Write %lx %lx %ld\r\n", src, dest, len);
//     return 0;
// }

// uint16_t dfu_erase_flash(uint32_t add)
// {
//     LOG_I("DFU Erase %lx\r\n", add);
//     return 0;
// }

// void dfu_leave(void)
// {
//     LOG_I("DFU Leave\r\n");
//     dfu_reset();
// }
