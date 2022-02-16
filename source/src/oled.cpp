// cam_wiz V0.7 220214 qrt@qland.de

#include "oled.h"

uint8_t* Oled::vidBuf;              
uint8_t* Oled::vidTra;              
uint8_t Oled::oledX, Oled::oledY;
uint8_t* Oled::briBuf;             
uint8_t Oled::hato;   
void (*Oled::dispFunc)() = &Oled::disp22;

struct halfTone22 {                         // halftone 2x2
    uint8_t brid;                           // brightness default
    uint8_t bric;                           //            custom
    uint8_t pat[2];                         // pattern
};          

struct halfTone33 {                         // halftone 3x3
    uint8_t brid;                           // brightness default
    uint8_t bric;                           //            custom
    uint8_t pat[3];                         // pattern
};

static struct halfTone22 ht22[] = {         // default fixed polynom y = -3.333x^3 + 19.643x^2 + 17.262x + 80.286  
    {  80,  80,                   { 0b00,   // custom preset linear  y = (w - b) / n * x + b, n=4, b=8, w=249
                                    0b00 }},  
              
    { 115, 122,                   { 0b01, 
                                    0b00 }},
              
    { 165, 165,                   { 0b01, 
                                    0b10 }},
              
    { 220, 207,                   { 0b11, 
                                    0b10 }},

    { MAX_GREY_565, MAX_GREY_565, { 0b11, 
                                    0b11 }}
};

static struct halfTone33 ht33[] = {         // default fixed ploynom y = -0.542x^3 + 7.365x^2 + 1.931x + 30.253
    {  30,  30,                   { 0b000,  // custom preset linear  y = (w - b) / n * x + b, n=9, b=30, w=249
                                    0b000, 
                                    0b000 }},                 
          
    {  39,  54,                   { 0b000, 
                                    0b010, 
                                    0b000 }},
          
    {  59,  79,                   { 0b000, 
                                    0b010, 
                                    0b001 }},
          
    {  88, 103,                   { 0b000, 
                                    0b010, 
                                    0b101 }},
          
    { 121, 127,                   { 0b100, 
                                    0b010, 
                                    0b101 }},
          
    { 156, 152,                   { 0b101, 
                                    0b010, 
                                    0b101 }},
          
    { 190, 176,                   { 0b111, 
                                    0b101, 
                                    0b010 }},
          
    { 219, 200,                   { 0b111, 
                                    0b101, 
                                    0b110 }},
          
    { 240, 225,                   { 0b111, 
                                    0b101, 
                                    0b111 }},

    { MAX_GREY_565, MAX_GREY_565, { 0b111, 
                                    0b111, 
                                    0b111 }}
};

// ht44
// polynom y = -0.117x^3 + 2.634x^2 + 2.232x + 20.286

void Oled::init()
{
    vidTra = (uint8_t*)malloc(VIDSIZE + 1);
    vidBuf = vidTra + 1;
    vidTra[0] = OLDATA;

    briBuf = (uint8_t*)malloc(128 * 64); 

    i2cInit();
    sleep_ms(1);

    i2cWrite((uint8_t*)initSeq, sizeof(initSeq));
    clear();
}

void Oled::greyWedge()
{   
    for(uint8_t x=0; x<128; x++)
        for(uint8_t y=0; y<64; y++)
            setBri(x, y, x<<1);
}

void Oled::calcHalfTone(uint8_t black, uint8_t white)
{        
    if(hato == C22){
        for(uint8_t x=0; x<5; x++)
            ht22[x].bric = (((white - black) * x) / 4 + black);
    }
    else if(hato == C33){
        for(uint8_t x=0; x<9; x++)
            ht33[x].bric = (((white - black) * x) / 8 + black);
    }
}

void Oled::setDisplay(uint8_t h)
{
    hato = h;

    if(h==D22 || h==C22)
        dispFunc = &disp22;        
    else if(h==D33 || h==C33)
        dispFunc = &disp33;
    else if(h == NOI)
        dispFunc = &dispNoise;
}

void Oled::display()
{        
    setXY(0, 0);

    (*dispFunc)();
    
    if(Keys::showTime()){
        char buf[17];
        sprintf(buf, "%s %s", Keys::getOptionCaption(), Keys::getOptionValueText());
        clrLine(46, 4);
        Text::writeText(buf, 0, 50);
        fillRight(Text::getPosX(), 50, 14);
    }

    i2cWrite(vidTra, VIDSIZE + 1);
}

void Oled::disp22()
{        
    for(uint8_t x=0; x<128; x+=2){
        for(uint16_t y=0; y<64; y+=2){
            for(uint8_t h=0; h<5; h++){
                if(getBri(x, y) <= (hato==D22 ? ht22[h].brid : ht22[h].bric)){
                    set22(x, y, h);
                    break;
                }
            }
        }
    }
}

void Oled::disp33()
{    
    for(uint8_t x=0; x<128; x+=3){
        for(uint16_t y=0; y<64; y+=3){
            for(uint8_t h=0; h<10; h++){
                if(getBri(x, y) <= (hato==D33 ? ht33[h].brid : ht33[h].bric)){
                    set33(x, y, h);
                    break;
                }
            }
        }
    }
}

void Oled::dispNoise()
{    
static uint16_t ya;
static uint8_t b, bb;
static uint8_t ma;
static uint8_t mi;

    for(uint16_t y=0; y<64; y++){
        ya = ((y>>3)<<7);
        ma = 1 << (y & 0x07);
        mi = ~ma;

        for(uint8_t x=0; x<128; x++){
            b = bb = vidBuf[ya + x];
          
            if((rand() & 0xff) > getBri(x ,y))
                b &= mi;
            else    
                b |= ma;
          
            if(b != bb)
                vidBuf[ya + x] = b;
        }
    }
}

void Oled::set22(uint8_t x, uint8_t y, uint8_t h)
{  
static uint16_t ya;
static uint8_t b, bb;
static uint8_t ma;
static uint8_t mi;
static uint8_t pat; 

    for(uint8_t yo=0; yo<2; yo++){
        ya = ((y+yo) & 0xf8) << 4;
        ma = 1 << ((y+yo) & 0x07);
        mi = ~ma;        
        pat = ht22[h].pat[yo];

        for(uint8_t xo=0; xo<2; xo++){
            b = bb = vidBuf[ya + x + xo];    
            
            if(pat & 1)
                b |= ma;
            else
                b &= mi;

            if(b != bb)
                vidBuf[ya + x + xo] = b;
            
            pat >>= 1;
        }
    }
}

void Oled::set33(uint8_t x, uint8_t y, uint8_t h)
{  
static uint16_t ya;
static uint8_t b, bb;
static uint8_t ma;
static uint8_t mi;
static uint8_t pat; 

    for(uint8_t yo=0; yo<3; yo++){
        ya = ((y+yo) & 0xf8) << 4;
        ma = 1 << ((y+yo) & 0x07);
        mi = ~ma;        
        pat = ht33[h].pat[yo];

        for(uint8_t xo=0; xo<3; xo++){
            b = bb = vidBuf[ya + x + xo];    
            
            if(pat & 1)
                b |= ma;
            else
                b &= mi;

            if(b != bb)
                vidBuf[ya + x + xo] = b;
            
            pat >>= 1;
        }
    }
}

void Oled::set(uint8_t x, uint8_t y, uint8_t pix)
{
static uint16_t ya;
static uint8_t ma; 
static uint8_t b, bb;

    ya = (y & 0xf8) << 4;
    ma = 1 << (y & 0x07);    
    b = bb = vidBuf[ya + x];

    if(pix)
        b |= ma;
    else
        b &= ~ma;

    if(b != bb)
        vidBuf[ya + x] = b;
}

void Oled::clrLine(uint8_t ys, uint8_t h)
{
    for(uint8_t y=ys; y<ys+h; y++)
        for(uint8_t x=0; x<128; x++)
            set(x, y, 0);
}

void Oled::fillRight(uint8_t xs, uint8_t ys, uint8_t h)
{
    for(uint8_t y=ys; y<ys+h; y++)
        for(uint8_t x=xs; x<128; x++)
            set(x, y, 0);
}

void Oled::setBri(uint8_t x, uint8_t y, uint8_t grey)
{
    *(briBuf + (y<<7) + x) = grey;
}

uint8_t Oled::getBri(uint8_t x, uint8_t y)
{
    return *(briBuf + (y<<7) + x);
}

void Oled::clear()
{      
    uint8_t *p = vidBuf;

    while(p < vidBuf+VIDSIZE)
        *p++ = 0;

    i2cWrite(vidTra, VIDSIZE + 1);
}

void Oled::setXY0()
{
    setXYs(0, 0);
}

void Oled::setXYs(uint8_t x, uint8_t y)
{
    oledX = x;
    oledY = y;

    setXY(x, y);
}

void Oled::setXY(uint8_t x, uint8_t y)
{
    uint8_t buf[5];

    buf[0] = OLCOMA;
    buf[1] = SET_PAGE_START_ADDRESS | y;    // y 0..7, (reset, end=7)

    buf[2] = SET_COLUMN_ADDRESS;            // x 0..127
    buf[3] = x;                             // column start
    buf[4] = 0x7f;                          //        end

    // buf[3] = SET_PAGE_ADDRESS;    // y 0..7, (reset, end=7)
    // buf[4] = y;
    // buf[5] = 7;
 
    i2cWrite(buf, 5);
}

void Oled::i2cInit()
{
    i2c_init(i2c_default, I2CFREQ);
    
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);    
} 

void Oled::i2cWrite(uint8_t* buf, size_t len)
{
    i2c_write_blocking(i2c_default, OLED_I2C_ADDR, buf, len, false);
}
