// cam_wiz V0.7 220214 qrt@qland.de

#pragma once

#include <string.h>
#include "pico/stdlib.h"
#include "cam.h"
#include "oled.h"

class Option
{
    public:
        Option();
        Option(const char *caption, uint16_t cmd);
               
        uint8_t getValue() { return value; }
        uint8_t getValueCam() { return Cam::readRegister(cmd); }
        void setValueCam(uint8_t v) { Cam::writeRegister(cmd, v); }
        const char *getCaption() { return caption; }
        void show(bool v) { visible = v; }
        bool isVisible() { return visible; }
        void setText(const char* text) { this->text = text; }
        void setCoOpt(Option *showOpt0, Option *showOpt1, Option *limOpt) { this->showOpt0=showOpt0; this->showOpt1=showOpt1; this->limOpt=limOpt; }

        char* getValueText();

        virtual void setValue(uint8_t value) { }
        virtual void changeValue(int8_t c) { };

    protected:
        uint16_t cmd;
        uint8_t value;
        bool visible;
        Option *showOpt0, *showOpt1, *limOpt;        

    private:
        const char *text;
        const char *caption;
};

class FlagOption : public Option
{
    public:
        FlagOption(const char *caption, uint16_t cmd, uint8_t mask); 

        virtual void setValue(uint8_t value) { this->value = (value & mask) == mask; }     
        virtual void changeValue(int8_t c);
        
    private:
        uint8_t mask;
};

class ValueOption : public Option
{
    public:
        ValueOption(const char *caption, uint16_t cmd, uint8_t min, uint8_t max, uint8_t step);

        virtual void setValue(uint8_t value) { this->value = value; }
        virtual void changeValue(int8_t c);

    private:
        uint8_t min, max, step;
};
