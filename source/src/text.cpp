// cam_wiz V0.7 220214 qrt@qland.de

#include "text.h"

uint8_t Text::xstart, Text::ystart;
uint8_t* Text::p;
uint8_t* Text::fbp;
uint8_t Text::b, Text::rb;                              
int8_t Text::rc;                                  

void Text::init()
{
}

void Text::writeText(char* text, uint8_t x, uint8_t y)
{
    char* p = text;
    xstart = x;
    ystart = y;

    while(*p != 0)
        writeChar(*p++);
}

void Text::writeChar(char c)
{
    p = (uint8_t*)oled_814;                         // font table pointer
    int8_t hight = *p++;                            // table +0  hight (max 127)
    uint8_t spacing = *p++;                         //       +1  spacing
    fbp = p;                                        //       +2  frequent bytes table
    p += COMPFB;                                    //           first char

    while(*p != c){                                 // char +0   code
        if(*p == 0)                                 //      +1   width
            return;                                 //      +2   data len
                                                    //      +3.. data
        p += *(p + 2) + 3;                          // + data len = next char
    };

    uint8_t width = *++p;                           // char width   
    p += 2;                                         //      data start

    uint8_t xpos;                                   // start position
    uint8_t ypos = ystart;                          //

    b = rb = rc = 0;                                // byte, repeat byte, repeat counter

    while(hight > 0){                               // row by row
        xpos = xstart;                              // start row

        for(uint8_t w=0; w<width; w++){             // column by column
            deComp();                               // decompress 
            drawByte(xpos++, ypos, b, hight>7 ? 8 : hight);
        }

        for(uint8_t i=0; i<spacing; i++)            // spacing
            drawByte(xpos++, ypos, 0, hight>7 ? 8 : hight);

        ypos += 8;                                  // next row
        hight -= 8;
    }

    xstart += width + spacing;                      // next char
}

#if COMPFB == 15                                    // not implemented correctly

void Text::deComp()
{
    if(rc == 0){                            // repeat counter down?
        b = *p++;                           // next byte

        if(b < 0xf0){                       // frequent byte
           rc = b & 0x0f;                   // repeat count
           b = rb = *(fbp + (b >> 4));      // get byte
        }
        else{
            if(b & 0x08){                   // byte repeat
                rc = -(b & 0x07);           // number of raw bytes, negative
                b = *p++;                   // get byte
            }
            else{                           // raw byte
                rc = (b & 0x07) + 1;        // repeat counter
                b = rb = *p++;              // get byte
            }
        }
    }
    else if(rc < 0){                        // raw byte
        rc++;                               // repeat counter ++
        b = *p++;                           // get byte
    }
    else{                                   // repeat byte
        rc--;                               // repeat counter --
        b = rb;                             // get byte
    }
}

#elif COMPFB == 31

void Text::deComp()
{
    if(rc == 0){                            // repeat counter down?
        b = *p++;                           // next byte

        if(b < 0xf8){                       // frequent byte
           rc = b & 0x07;                   // repeat count
           b = rb = *(fbp + (b >> 3));      // get byte
        }
        else{                               // raw byte
            rc = -(b & 0x07);               // repeat counter
            b = *p++;                       // get byte
        }
    }
    else if(rc < 0){                        // raw byte
        rc++;                               // repeat counter ++
        b = *p++;                           // get byte
    }
    else{                                   // repeat frequent byte
        rc--;                               // repeat counter --
        b = rb;                             // get byte
    }
}

#endif

void Text::drawByte(uint8_t x, uint8_t y, uint8_t b, uint8_t h)
{
    for(uint8_t yy=y; yy<y+h; yy++){
        Oled::set(x, yy, b & 0x01);
        b >>= 1;
    }
}

uint8_t Text::getPosX()
{
    return xstart;
}
