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

void RingBufferRxCallback(RingBuf_t *cfg, uint16_t pos, uint8_t *buffer, uint16_t size, uint16_t max_size){
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
}
