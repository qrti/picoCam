// cam_wiz V0.7 220214 qrt@qland.de

#pragma once

#include "pico/stdlib.h"
#include "camCtrl.h"
#include "camData.h"
#include "camIni.h"

#define ADDRW       (0x42>>1)                               // register write
#define ADDRR       (0x43>>1)                               //          read

class Cam
{
    public:
        static void init(uint16_t xres, uint16_t yres);

        static uint8_t GBR422_Grey(uint16_t c);
        static uint8_t RGB444_Grey(uint16_t c);       
        static uint8_t RGB565_Grey(uint16_t rgb);

        static void writeRegister(uint8_t reg, uint8_t data);
        static uint8_t readRegister(uint8_t reg);

    private:        
        static uint16_t xres, yres;
};
