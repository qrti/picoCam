// cam_wiz V0.7 220214 qrt@qland.de

#include "quart.h"

void QUart::init()
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_fifo_enabled(UART_ID, false);
    setbuf(stdout, NULL);
}

void putc(char c)
{
    uart_putc(UART_ID, c);
}

// void QUart::print(const char* format, ...)
// {
//     char buf[64];
// 
//     va_list arg;
//     va_start(arg, format);
//     vsprintf(buf, format, arg); 
//     va_end(arg);
// 
//     uart_puts(UART_ID, buf);
// }
