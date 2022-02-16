// cam_wiz V0.7 220214 qrt@qland.de

#include "camCtrl.h"

void CamCtrl::init()
{
    i2c_init(i2c_inst, 400*1000);
    
    gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_I2C_SDA_PIN);
    gpio_pull_up(PICO_I2C_SCL_PIN);
}

void CamCtrl::writeRegister(uint8_t i2cAd, uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = { reg, data };
    i2c_write_blocking(i2c_inst, i2cAd, buf, 2, false);
}

uint8_t CamCtrl::readRegister(uint8_t i2cAd, uint8_t reg)
{
    uint8_t buf[1] = { 0 };
    i2c_write_blocking(i2c_inst, i2cAd, &reg, 1, false);           
    i2c_read_blocking(i2c_inst, i2cAd, buf, 1, false);
    return buf[0];
}
