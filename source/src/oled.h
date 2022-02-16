// cam_wiz V0.7 220214 qrt@qland.de

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "keys.h"
#include "text.h"

#define OLED_I2C_ADDR               _u(0x78>>1)

#define OLCOMA                      _u(0x00)
#define OLDATA                      _u(0x40)

#define SET_CONTRAST_CONTROL        _u(0x81)
#define SET_DISPLAY                 _u(0xae)
#define SET_DISPLAY_NORMAL          _u(0xa6)
#define SET_DISPLAY_INVERSE         _u(0xa7)

#define ENTIRE_DISPLAY_FORAM        _u(0xa4)    // display follow RAM
#define ENTIRE_DISPLAY_IGRAM        _u(0xa5)    //         ignore

#define SET_MEMORY_ADDR_MODE        _u(0x20)
#define HORIZONTAL_ADDRESSING_MODE  _u(0x00)
#define VERTICAL_ADDRESSING_MODE    _u(0x01)
#define PAGE_ADDRESSING_MODE        _u(0x02)

#define SET_PAGE_START_ADDRESS      _u(0xb0)    // no default, init page start 0
#define SET_COLUMN_ADDRESS          _u(0x21)    
#define SET_PAGE_ADDRESS            _u(0x22)    // default start 0, end 7, no init

#define SET_COM_OUTPUT              _u(0xc0)

#define SET_DISPLAY_OFFSET          _u(0xd3)
#define SET_DISPLAY_CLOCK           _u(0xd5)
#define SET_PRECHARGE_PERIOD        _u(0xd9)
#define SET_COM_PINS                _u(0xda)

#define SET_VCOMH_DESELECT          _u(0xdb)    // _u(0x00, _u(0x20, _u(0x30

#define SET_LOWER_COLUMN            _u(0x00)
#define SET_HIGHER_COLUMN           _u(0x10)
#define SET_DISPLAY_START_LINE      _u(0x40)    // default 0, init 0

#define SET_SEGMENT_REMAP           _u(0xa0)    

#define SET_MULTIPLEX_RATIO         _u(0xa8)
#define COMMAND_NO_OPERATION        _u(0xe3)

#define CHARGE_BUMB_SETTING         _u(0x8d)
#define SET_SCROLL                  _u(0x2E)

// #define i2c_default PICO_DEFAULT_I2C_INSTANCE

#define I2CFREQ                     (2000 * 1000)
#define VIDSIZE                     (128 * 64 / 8)

#define MAX_GREY_565    ((0xf8 + 0xfc + 0xf8) / 3)

#define D22     0               // halftone default 2x2
#define D33     1               //                  3x3
#define NOI     2               //          noise
#define C22     3               //          custom  2x2
#define C33     4               //                  3x3
#define NUM_HT  5

const uint8_t initSeq[] = {                     // SSD1306 Oled controller init sequence
    SET_DISPLAY             | 0x00,
    SET_MEMORY_ADDR_MODE, HORIZONTAL_ADDRESSING_MODE,

    SET_DISPLAY_START_LINE  | 0x00, 
    SET_SEGMENT_REMAP       | 0x01, 
    SET_MULTIPLEX_RATIO,    _u(64-1),
    SET_COM_OUTPUT          | 0x08, 
    SET_DISPLAY_OFFSET, 	_u(0x00), 

    SET_COLUMN_ADDRESS,     _u(0), _u(127), 
    SET_PAGE_ADDRESS,       _u(0), _u(7),

    // SET_COM_PINS,           _u(0x02),   // _u(0x12),
    // SET_DISPLAY_CLOCK,      _u(0x80),   // _u(0xf0), 
    // SET_PRECHARGE_PERIOD,   _u(0xf1),   // _u(0x22),
    // SET_VCOMH_DESELECT,	    _u(0x30),   // _u(0x20),

    SET_COM_PINS,           _u(0x12),
    SET_DISPLAY_CLOCK,      _u(0xf0), 
    SET_PRECHARGE_PERIOD,   _u(0x22),
    SET_VCOMH_DESELECT,	    _u(0x20),

    SET_CONTRAST_CONTROL,   _u(0xff),    
    ENTIRE_DISPLAY_FORAM, 
    SET_DISPLAY_NORMAL,
    CHARGE_BUMB_SETTING,    _u(0x14),
    SET_SCROLL              | 0x00,
    SET_DISPLAY             | 0x01
};

class Oled
{
    public:
        static void init();
        static void greyWedge();
        static void setDisplay(uint8_t d);
        static void display();
        static void setBri(uint8_t x, uint8_t y, uint8_t grey);
        static void set(uint8_t x, uint8_t y, uint8_t pix);
        static void clrLine(uint8_t ys, uint8_t h);
        static void fillRight(uint8_t xs, uint8_t ys, uint8_t h);

        static void calcHalfTone(uint8_t black, uint8_t white);

        static void clear();
        static void setXY0();
        static void setXYs(uint8_t, uint8_t);
        static void setXY(uint8_t, uint8_t);

    private:
        static void disp22();
        static void disp33();
        static void dispNoise();
        static void set22(uint8_t x, uint8_t y, uint8_t p);
        static void set33(uint8_t x, uint8_t y, uint8_t p);
        static uint8_t getBri(uint8_t x, uint8_t y);

        static void i2cInit();
        static void i2cSendCmd(uint8_t);
        static void i2cWrite(uint8_t*, size_t);

        static void (*dispFunc)();              // pointer to display function
        static uint8_t* briBuf;                 // brightness buffer
        static uint8_t* vidBuf;                 // video buffer
        static uint8_t* vidTra;                 // video transfer buffer
        static uint8_t oledX, oledY;
        static uint8_t hato;
};
