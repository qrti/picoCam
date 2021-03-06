// cam_wiz V0.7 220214 qrt@qland.de
// Camera OV7725 -> RB2040 Pico -> OLED Display SSD1306

/*             ____ main ____
             /         \      \ 
          cam         oled      keys
         /   \          |        |
    camCtrl  camData   text    option

    core 0
    cam data    -> PIO -> every 4th line -> DMA -> line buffer -> convert -> grey buffer
    8 bit width           256 to 64                512x2 byte     RGB565     128x64 byte
    512x256 pix                                    one line                  64 lines
    
    -> VSYNC IR -> reset line counter
    -> HREF IR  -> advance line counter, start DMA
    -> 10 ms key polling routine for options

    core 1
    grey buffer -> halftone filter        -> video buffer -> I2C   -> OLED display
    128x64 byte    2x2 - 3x3 - 1x1 noise |   128x64 bit      2 MHz    128x64 pix monochrome
                                         |
                        option overlay ->
*/

// options (overlay display)
// - brightness, contrast
// - halftone patterns D2x2 - D3x3 - 1x1 noise - C2x2 - C3x3
//   D = default polynomial distribution, C = custom linear distribution
// - halftone black and white offset
//   only visible for custom halftone patterns C2x2 and C3x3
// - night vision
// - vertical-flip, horizontal-flip

// keys
// - show/next option
// - previous, minus
// - next, plus

// remarks
// - camera OV7725 no fifo version (without RAM on PCB) is used for this project
// - if the picture is distorted the camera might not have powered up properly
//   try Pico reset (RUN (pin 30) to GND) or power up again
// - OLED display might stuck in I2C handshake on Pico reset, try power up again
// - OLED I2C frequency is tuned from default 400 KHz to 2 MHz to make the display more fluent
//   if the display tends to stuck or is streaky try to reduce I2CFREQ in oled.h
// - halftone patterns 3x3 are wrapped from last to first row
// - black/white halftone offsets for custom 2x2 and 3x3 are the same
// - extreme black/white haltftone offset values might produce strange display effects
// - night vision might not switch off properly, reason unknown
// - attached python scripts invoke the RP2040 assembler on .pio file changes when compiling
//   see platform.ini, pre_script.py and post_script.py
// - quart is for simple debugging with printf on serial TX (pin 1) and RX (pin 2)

// developed and compiled with vsCode, platformIO and wizio-pico
// https://code.visualstudio.com/
// https://platformio.org/
// https://github.com/Wiz-IO/wizio-pico 

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
// #include "quart.h"              // printf debug
#include "cam.h"
#include "oled.h"
#include "keys.h"

void core1_entry();

uint16_t volatile nLine = 0xffff;

int main(void)
{
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // QUart::init();              // printf debug
    Cam::init(512, 256);
    Oled::init();
    Oled::greyWedge();
    Keys::init();

    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    multicore_launch_core1(core1_entry);
    sleep_ms(500);

    uint32_t* cbuf = CamData::capture_buf;
    uint32_t* p;
    uint8_t y;

    while(true){
        if(nLine != 0xffff){                                    // wait for new line
            if((nLine & 0x03) == 0){                            // every 4th line
                y = nLine >> 2;                                 // line /4 for oled resolution
                p = cbuf;                                       // refresh buffer pointer

                CamData::startDma();                            // init pio + dma, wait for HREF==1, start pio
                sleep_us(20);                                   // prevent reading dma buffer before first bytes are written

                for(uint8_t x=0; x<128; x++){                   // convert dma buffer and store to oled grey buffer
                    Oled::setBri(x, y, Cam::RGB565_Grey(*p));   // ignore *p >> 16
                    p += 2;                                     // every 4th pixel
                }

                nLine = 0xffff;                                 // devalue new line
            }
        }

        Keys::handle();
    }
}

void core1_entry()
{
    while(true)
        Oled::display();
}
