#include "ring_buffer.h"

#include <string.h>

void RingBufferInit(RingBuf_t *cfg, uint8_t *buffer, uint16_t length){
    memset(buffer, 0x00, length);

    cfg->DataIn  = &cfg->DataPtr[0];
    cfg->DataOut = &cfg->DataPtr[0];
    cfg->DataEnd = &cfg->DataPtr[RING_BUF_NUM-1];
    cfg->DataIn->start = buffer;
    cfg->Counter = 0;
}

bool RingBufferTxCallback(RingBuf_t *cfg, void(*dma_transmit)(void)){
    bool is_busy = false;

    if (cfg->DataOut != cfg->DataIn){
        is_busy = true;
        dma_transmit();
        cfg->DataOut ++;
        if (cfg->DataOut == cfg->DataEnd){
            cfg->DataOut = &cfg->DataPtr[0];
        }
    }

    return is_busy;
}

void RingBufferRxCallback(RingBuf_t *cfg, uint16_t pos, uint8_t *buffer, uint16_t size, uint16_t max_size, void(*dma_receive)(void)){
    cfg->Counter += pos;
    cfg->DataIn->end = &buffer[cfg->Counter-1];
    cfg->DataIn ++;
    if (cfg->DataIn == cfg->DataEnd){
        cfg->DataIn  = &cfg->DataPtr[0];
    }
    if ((size-cfg->Counter) >= max_size){
        cfg->DataIn->start = &buffer[cfg->Counter];
    }
    else {
        cfg->DataIn->start = buffer;
        cfg->Counter = 0;
    }
    dma_receive();
}

bool RingBufferRxHandle(RingBuf_t *cfg, void(*handle)(uint8_t *data, uint16_t length)){
    bool is_handle = false;

    if (cfg->DataOut != cfg->DataIn){
        handle(cfg->DataOut->start, cfg->DataOut->end - cfg->DataOut->start+1);
        is_handle = true;
        cfg->DataOut ++;
        if (cfg->DataOut == cfg->DataEnd){
            cfg->DataOut = &cfg->DataPtr[0];
        }
    }

    return is_handle;
}
