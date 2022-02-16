// cam_wiz V0.7 220214 qrt@qland.de

#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_INSTANCE        1
#define i2c_inst            (__CONCAT(i2c, I2C_INSTANCE))   // i2c1

#define PICO_I2C_SDA_PIN    6
#define PICO_I2C_SCL_PIN    7

class CamCtrl
{
    public:
        static void init();
        static void writeRegister(uint8_t i2cAd, uint8_t reg, uint8_t data);
        static uint8_t readRegister(uint8_t i2cAd, uint8_t reg);
};
