// cam_wiz V0.7 220214 qrt@qland.de

#pragma once

#include "pico/stdlib.h"
#include <string>
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

void putc(char c);

class QUart
{
    public:
        static void init();
};


/*! \brief Binary formated output -
* 
* fBin(var).c_str())
*
* \param x uint_8..32 variable
*/
template<class T>
inline std::string fBin(T x)
{
    char b[sizeof(T)*9+1] = { 0 };
    char* p = b + sizeof(T) * 9;

    for(uint8_t z=0; z<sizeof(T)*8; z++){
        if(z%8==0) *--p = ' ';
        *--p = ((x>>z) & 1) ? '1' : '0';
    }

    return std::string(b);
}

// template<class T>
// inline std::string format_binary(T x)
// {
//     char b[sizeof(T)*8+1] = { 0 };
//
//     for(size_t z=0; z<sizeof(T)*8; z++)
//         b[sizeof(T)*8-1-z] = ((x>>z) & 0x1) ? '1' : '0';
//
//     return std::string(b);
// }
