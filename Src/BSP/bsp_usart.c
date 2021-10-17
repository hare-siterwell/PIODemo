/**
 * @file bsp_usart.c
 * @brief USART
 */

#include "bsp_usart.h"

struct UsartRx ur1;

/**
 * @brief Enable USART
 */
void USART_DMA_Enable(void) {
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_5,
                          LL_USART_DMA_GetRegAddr(USART1));
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_5, (u32)ur1.buf);
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, USART_RXSIZE);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
  LL_USART_EnableDMAReq_RX(USART1);
  LL_USART_ClearFlag_IDLE(USART1);
  LL_USART_EnableIT_IDLE(USART1);

  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_4,
                          LL_USART_DMA_GetRegAddr(USART1));
  LL_USART_EnableDMAReq_TX(USART1);
}

/**
 * @brief Rx Transfer IDLE callbacks
 * @param USARTx Universal Synchronous Asynchronous Receiver Transmitter
 */
void USART_RxIdleCallback(USART_TypeDef *USARTx) {
  OS_ERR err;
  if (USARTx == USART1 && LL_USART_IsActiveFlag_IDLE(USART1)) {
    LL_USART_ClearFlag_IDLE(USART1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
    ur1.len = USART_RXSIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
    if (ur1.len) {
      OSSemPost(&ur1.sta, OS_OPT_POST_1, &err); // Processing data
    } else {
      USART_ReEnable(USART1);
    }
  }
}

/**
 * @brief Re-enable DMA mode for reception
 * @param USARTx Universal Synchronous Asynchronous Receiver Transmitter
 */
void USART_ReEnable(USART_TypeDef *USARTx) {
  if (USARTx == USART1) {
    memset(ur1.buf, 0, ur1.len);
    ur1.len = 0;
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, USART_RXSIZE);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
  }
}

/**
 * @brief Sends an amount of data in DMA mode
 * @note Pay attention to DMA access address permissions
 * @param USARTx Universal Synchronous Asynchronous Receiver Transmitter
 * @param pData Pointer to data buffer
 * @param Size Amount of data elements
 */
void USART_Send(USART_TypeDef *USARTx, u8 *pData, u32 Size) {
  if (USARTx == USART1) {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (u32)pData);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, Size);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
  }
}

/* Support "printf()" */
#ifdef __GNUC__
int _write(int fd, char *pBuffer, int size) {
  for (int i = 0; i < size; i++) {
    while (!LL_USART_IsActiveFlag_TXE(USART1))
      ;
    LL_USART_TransmitData8(USART1, pBuffer[i]);
  }
  return size;
}
#else
FILE __stdout;
void _sys_exit(int x) {}
int fputc(int ch, FILE *f) {
  LL_USART_TransmitData8(USART1, ch);
  while (!LL_USART_IsActiveFlag_TXE(USART1))
    ;
  return ch;
}
#endif
