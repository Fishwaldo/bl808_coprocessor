
#include <stdarg.h>
#include <bflb_core.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <shell.h>
#include <ring_buffer.h>
#include "console.h"
#include "usb_uart.h"
#include "cmd/cmd_sensors.h"
#include "cmd/cmd_system.h"
#ifdef CONFIG_TLSF
#include "cmd/cmd_memory.h"
#endif

/* List of Our Commands to Include */
SHELL_CMD_EXPORT_ALIAS(cmd_voltage, voltage, get voltage);
SHELL_CMD_EXPORT_ALIAS(cmd_temp, temp, get temperature);
SHELL_CMD_EXPORT_ALIAS(cmd_sysinfo, sysinfo, get system info);
#ifdef CONFIG_TLSF
SHELL_CMD_EXPORT_ALIAS(cmd_meminfo, meminfo, get free memory info);
#endif


/* need to add to shell SDK header */
extern void shell_init_with_task(struct bflb_device_s *shell);
extern int shell_set_print(void (*shell_printf)(char *fmt, ...));




void console_init() {
    /* start a UART Shell */
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_set_prompt("0x64>");
    shell_init_with_task(uart0);
}
