// cam_wiz V0.7 220214 qrt@qland.de

#pragma once

#include "pico/stdlib.h"
#include "option.h"

#define MINUS_PIN           26
#define PLUS_PIN            27
#define OPT_PIN             28
#define KEY_MASK            (1<<OPT_PIN | 1<<PLUS_PIN | 1<<MINUS_PIN)

#define KEY_CYCLE           10                      // 10 ms
#define KEY_LONG            (2000 / KEY_CYCLE)      // 2 s
#define KEY_SHORT           (50 / KEY_CYCLE)        // 50 ms
#define SHOWTIME            (5000 / KEY_CYCLE)      // 5 s

#define OPT_BRIGHT          0
#define OPT_CONTRAST        1
#define OPT_HTPAT           2
#define OPT_HTOBL           3
#define OPT_HTOWH           4
#define OPT_NIGHT           5
#define OPT_VFLIP           6
#define OPT_HFLIP           7
#define NUM_OPTS            8
#define DEFAULT_OPT         OPT_BRIGHT

#define IFU_HTPAT           (1<<8)                  // internal function, halftone pattern 
#define IFU_HTOBL           (2<<8)                  //                             offset black
#define IFU_HTOWH           (3<<8)                  //                                    white

class Keys
{
    public:
        static void init();
        static void handle();        
        
        static bool showTime() { return showCnt; }
        static const char* getOptionCaption();
        static char* getOptionValueText();

    private:
        static void keyShort(uint32_t keys);
        static void keyLong(uint32_t keys);

        static class Option* option[];
        static uint8_t opt;
        static uint8_t keyCnt;
        static uint32_t keyCyc;
        static uint32_t keys;
        static uint16_t showCnt;
};
