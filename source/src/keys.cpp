// cam_wiz V0.7 220214 qrt@qland.de

#include "keys.h"

uint8_t Keys::opt = DEFAULT_OPT;
uint8_t Keys::keyCnt;
uint32_t Keys::keyCyc;
uint32_t Keys::keys;
Option *Keys::option[NUM_OPTS];
uint16_t Keys::showCnt;

void Keys::init()
{
    gpio_init_mask(KEY_MASK);                                   // init key GPIOs
    gpio_set_dir_in_masked(KEY_MASK);                           // input
    gpio_set_pulls(MINUS_PIN, true, false);                     // pullup
    gpio_set_pulls(PLUS_PIN, true, false);                      // pullup
    gpio_set_pulls(OPT_PIN, true, false);                       // pullup

    option[OPT_BRIGHT]   = new ValueOption("BRIGHT:", BRIGHT,    0, 255, 8);
    option[OPT_CONTRAST] = new ValueOption("CONTRA:", CNST,      0, 255, 8);    
    option[OPT_HTPAT]    = new ValueOption("HTPAT: ", IFU_HTPAT, 0,   4, 1);
    option[OPT_HTOBL]    = new ValueOption("HTOFBL:", IFU_HTOBL, 0, 255, 8);    
    option[OPT_HTOWH]    = new ValueOption("HTOFWH:", IFU_HTOWH, 0, 255, 8);    
    option[OPT_NIGHT]    = new FlagOption("NIGHT: ",  COM5,      1<<7);
    option[OPT_VFLIP]    = new FlagOption("VFLIP: ",  COM3,      1<<7);
    option[OPT_HFLIP]    = new FlagOption("HFLIP: ",  COM3,      1<<6);

    option[OPT_HTOBL]->show(false);
    option[OPT_HTOWH]->show(false);
    
    option[OPT_HTPAT]->setCoOpt(option[OPT_HTOBL], option[OPT_HTOWH], NULL);
    option[OPT_HTOBL]->setCoOpt(NULL, NULL, option[OPT_HTOWH]);
    option[OPT_HTOWH]->setCoOpt(NULL, NULL, option[OPT_HTOBL]);

    option[OPT_HTOBL]->setValue(80);   
    option[OPT_HTOWH]->setValue(248);

    option[OPT_HTPAT]->setText("D22\0D33\0NOI\0C22\0C33\0");
    option[OPT_NIGHT]->setText("OFF\0ON \0");
    option[OPT_VFLIP]->setText("OFF\0ON \0");
    option[OPT_HFLIP]->setText("OFF\0ON \0");
}

void Keys::handle()
{
    if(time_us_32() > keyCyc){
        uint32_t k = gpio_get_all() & KEY_MASK;                 // get keys

        if(k != KEY_MASK){                                      // key pressed
            if(keyCnt == 0)                                     // store newly pressed keys
                keys = k;
            
            if(++keyCnt == 0xff)                                // advance and restrict key counter
                keyCnt = 0xff;
            
            if(keyCnt == KEY_LONG)                              // key long press
                keyLong(keys);
        }
        else{                                                   // key released
            if(keyCnt<KEY_LONG && keyCnt>=KEY_SHORT)            // key short press  
                keyShort(keys);                             

            keyCnt = 0;                                         // reset key counter
        }

        if(showCnt > 0)
            showCnt--;
        
        keyCyc = time_us_32() + KEY_CYCLE * 1000;               // 10 ms
    }
}

void Keys::keyLong(uint32_t keys)                               // after key was pressed for 2 s
{
}

void Keys::keyShort(uint32_t keys)                              // after releasing key
{    
    if(!(keys & 1<<OPT_PIN)){  
        if(showCnt){
            uint8_t o;                                          // find next visible option
            
            for(o=1; o<NUM_OPTS; o++)
                if(option[(opt + o) % NUM_OPTS]->isVisible())
                    break;

            opt = (opt + o) % NUM_OPTS;
        }
    }
    else if(!(keys & 1<<MINUS_PIN)){
        option[opt]->changeValue(-1);
    }
    else if(!(keys & 1<<PLUS_PIN)){
        option[opt]->changeValue(1);
    }

    showCnt = SHOWTIME;
}    

const char* Keys::getOptionCaption() 
{ 
    return option[opt]->getCaption(); 
}

char* Keys::getOptionValueText() 
{ 
    return option[opt]->getValueText(); 
}
