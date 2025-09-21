#include "globals.h"
#include "systick.h"
#include "hal_debug.h"

uint16_t tx_Timer = 0;

void nvicInit(void){
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    // 全局时间中断
    NVIC_InitStructure.NVIC_IRQChannel = systickIrqChannel();
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    // 串口
    NVIC_InitStructure.NVIC_IRQChannel = debugIrqChannel();
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
}

int main(void){

    nvicInit();
    systickInit();
    debugInit(115200);
    systickLoading(&tx_Timer, 2000);

    while(1){
        if (systickGetSub(tx_Timer) == 0){
            debugPrintf("你好 CMake\r\n");
            systickLoading(&tx_Timer, 2000);
        }
        if (debug_cb.DataIn != debug_cb.DataOut){
            debugPrintf("本次接收了%d字节数据\r\n", debug_cb.DataOut->end - debug_cb.DataOut->start + 1);
            debugPrintf("%s\r\n", debug_cb.DataOut->start);
            debug_cb.DataOut ++;
            if (debug_cb.DataOut == debug_cb.DataEnd){
                debug_cb.DataOut = &debug_cb.DataPtr[0];
            }

        }
    }
}
