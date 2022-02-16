// cam_wiz V0.7 220214 qrt@qland.de

#include "camData.h"

uint16_t CamData::line;
PIO CamData::pio;   
uint CamData::sm;    
uint CamData::dma_chan; 
uint32_t* CamData::capture_buf;
dma_channel_config CamData::dcc;
uint16_t CamData::buf_size_words;

void CamData::init(uint16_t xres, uint16_t yres)
{
    gpio_init(RESET_PIN);                       // init pins
    gpio_set_dir(RESET_PIN, GPIO_OUT);          // reset output
    gpio_put(RESET_PIN, 1);                     // reset off

    gpio_init(VSYNC_PIN);                       // VSYNC input
    gpio_set_dir(VSYNC_PIN, GPIO_IN);           //
    gpio_set_pulls(VSYNC_PIN, false, false);    // no pullup, no pulldown

    gpio_init(HREF_PIN);                        // HREF input
    gpio_set_dir(HREF_PIN, GPIO_IN);            //
    gpio_set_pulls(HREF_PIN, false, false);     // no pullup, no pulldown

    buf_size_words = (xres * 2) / 4;            // 2 byte per pixel, 4 byte per word

    initXclk();                                 // start XCLK

//  sleep_ms(1);                                // reset
//  gpio_put(RESET_PIN, 0);                   
//  sleep_ms(1);                              
//  gpio_put(RESET_PIN, 1);                   
//  sleep_ms(1);

    initPio();                                  // init PIO
    initDma();                                  // init DMA    
    initHrefIR();                               // init HREF IR
    initVsyncIR();                              // init VSYNC IR    
}

void CamData::initPio()
{
    pio = pio0;
    uint offset = pio_add_program(pio, &parInput_program);
    sm = pio_claim_unused_sm(pio, true);
    parInput_program_init(pio, sm, offset, PIN_BASE, PIN_COUNT);   

    // pio_sm_set_enabled(pio, sm, false);
    // pio_sm_clear_fifos(pio, sm);
    // pio_sm_restart(pio, sm);
    // pio_sm_set_enabled(pio, sm, true);
}

void CamData::initDma()
{
    capture_buf = (uint32_t *)malloc(buf_size_words * sizeof(uint32_t));
    hard_assert(capture_buf);

    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    dma_chan = dma_claim_unused_channel(true);
    dcc = dma_channel_get_default_config(dma_chan);
    channel_config_set_read_increment(&dcc, false);
    channel_config_set_write_increment(&dcc, true);
    channel_config_set_dreq(&dcc, pio_get_dreq(pio, sm, false));
}

void CamData::startDma()
{
    // dma_channel_abort(dma_chan);
    pio_sm_set_enabled(pio, sm, false);     // stop pio
    pio_sm_clear_fifos(pio, sm);            // clear fifos
    pio_sm_restart(pio, sm);                // restart pio

    dma_channel_configure(dma_chan, &dcc,
        capture_buf,                        // destination pointer
        &pio->rxf[sm],                      // source pointer
        buf_size_words,                     // number of transfers
        true                                // start immediately
    );

    pio_sm_exec(pio, sm, pio_encode_wait_gpio(0, HREF_PIN));    // encoded and execute pio, wait for HREF_PIN==0, HREF reverse 
    pio_sm_set_enabled(pio, sm, true);                          // start pio
}

void CamData::initXclk()
{
    clock_gpio_init(XCLK_PIN, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, SYS_CLK_DIV);    // clock_stop(clock_index::clk_gpout0);
}

void CamData::initHrefIR()
{
    gpio_set_irq_enabled_with_callback(HREF_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
}

void CamData::initVsyncIR()
{
    gpio_set_irq_enabled(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, true);
}

void CamData::gpio_callback(uint gpio, uint32_t events) 
{    
    if(gpio == HREF_PIN){                           // last byte of line 
        nLine = line;
        line++;
    }
    else if(gpio == VSYNC_PIN){                     // line 0, VGA  first row in 22 lines
        line = 0;                                   //         QVGA              26
    }
}

void CamData::parInput_program_init(PIO pio, uint sm, uint offset, uint pin_base, uint pin_count)
{    
    pio_sm_config c = parInput_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin_base);         
    pio_sm_set_consecutive_pindirs(pio, sm, pin_base, pin_count, false);    

    for(uint8_t i=0; i<pin_count; i++){
        pio_gpio_init(pio, pin_base + i);    
        gpio_set_pulls(pin_base + i, false, false);
    }

    sm_config_set_in_shift(
        &c,         // config
        false,      // shift to left
        true,       // autopush enabled
        32          //          threshold
    );
    
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
}
