// cam_wiz V0.7 220214 qrt@qland.de

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "parInput.pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"

#define SYS_CLK_DIV    10                   // 125 MHz / SYS_CLK_DIV = 12.5 MHz (OV7725 10..48 MHz)
#define PIN_BASE        8                   // D0
#define PIN_COUNT       8                   // D0..D7

#define D0              8                   // in         
#define D1              9                   // in
#define D2              10                  // in
#define D3              11                  // in
#define D4              12                  // in
#define D5              13                  // in
#define D6              14                  // in
#define D7              15                  // in

#define PCLK_PIN        16                  // in
#define HREF_PIN        17                  // in
#define VSYNC_PIN       18                  // in
#define RESET_PIN       19                  // out L=RESET           
#define XCLK_PIN        21                  // out 25 MHz, possible: gpio 21 23 24 25 = clk_gpout 0  1  2  3

extern volatile uint16_t nLine;

class CamData
{
    public:
        static void init(uint16_t xres, uint16_t yres);
        static void startDma();

        static uint dma_chan; 
        static uint32_t *capture_buf;

    private:
        static void initPio();        
        static void initXclk();
        static void initDma();
        static void initHrefIR();
        static void initVsyncIR();
        static void gpio_callback(uint gpio, uint32_t events);
        static void href_callback(uint gpio, uint32_t events);
        static void parInput_program_init(PIO pio, uint sm, uint offset, uint pin_base, uint pin_count);

        static PIO pio;   
        static uint sm;    
        static dma_channel_config dcc;
        static uint16_t buf_size_words;
        static uint16_t line;
};
