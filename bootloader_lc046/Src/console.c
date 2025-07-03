#include "main.h"

// 实现 printf
#pragma import(__use_no_semihosting)

struct __FILE {
    int handle;
};
FILE __stdout;

void _sys_exit(int x) {
    x = x;
}

int fputc(int ch, FILE * f) {
    CONSOLE_UART->TXBUF = ch;
    while (!(CONSOLE_UART->ISR & 0x01));

    return (ch);
}

void console_init(void) {
    printf("\r\n/** bootloader begin **/\r\n");
}

