#ifndef CONSOLE_H
#define CONSOLE_H

void console_init(void);

#define INCLUDE_CMD_SYM(x) void* __ ## x ## _fp =(void*)&x;

#endif