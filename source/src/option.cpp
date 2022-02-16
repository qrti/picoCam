// cam_wiz V0.7 220214 qrt@qland.de

#include "option.h"

Option::Option(const char *caption, uint16_t cmd)
{
    this->caption = caption;
    this->cmd = cmd;
    this->visible = true;
    setText(NULL);
    setCoOpt(NULL, NULL, NULL);
}

char ovalt[4];

char *Option::getValueText()
{
    if(text){
        return (char *)text + getValue() * 4;
    }
    else{
        sprintf(ovalt, "%3d", getValue());
        return ovalt;
    }
}

FlagOption::FlagOption(const char *caption, uint16_t cmd, uint8_t mask) : Option(caption, cmd)
{
    this->mask = mask;
    setValue(getValueCam());
}

void FlagOption::changeValue(int8_t c) 
{ 
    uint8_t v = getValueCam();          // c unused
    v ^= mask;                          // invert mask
    sleep_ms(1);
    setValueCam(v);
    setValue(v);    
}

ValueOption::ValueOption(const char *caption, uint16_t cmd, uint8_t min, uint8_t max, uint8_t step) : Option(caption, cmd)
{    
    this->min = min;
    this->max = max;
    this->step = step;    
    value = cmd>0xff ? 0 : getValueCam();
}

void ValueOption::changeValue(int8_t c) 
{ 
    if(cmd == IFU_HTPAT){
        int16_t v = (int16_t)value + c;
        if(v < min) v = min;
        if(v > max) v = max; 
        Oled::setDisplay(v);
        showOpt0->show(v==C22 || v==C33);
        showOpt1->show(v==C22 || v==C33);
        value = v;
    }    
    else if(cmd == IFU_HTOBL){       
        int16_t black = (int16_t)value + c * step;
        uint8_t white = limOpt->getValue();
        if(black < min) black = min;
        if(black > white) black = white; 
        black = (black / step) * step;
        Oled::calcHalfTone(black, white);
        value = black;
    }
    else if(cmd == IFU_HTOWH){
        int16_t white = (int16_t)value + c * step;
        uint8_t black = limOpt->getValue();
        if(white < black) white = black;
        if(white > max) white = max; 
        white = (white / step) * step;
        Oled::calcHalfTone(black, white);
        value = white;
    }
    else{
        int16_t v = (int16_t)getValueCam() + c * step;    
        if(v < min) v = min;
        if(v > max) v = max;
        v = (v / step) * step;
        sleep_ms(1);                    // some delay seems to be necessary
        setValueCam(v);
        value = v;
    }
}
