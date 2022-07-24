/*
 * uart_comm.cpp
 *
 *  Created on: Jun 4, 2022
 *      Author: yumas
 */
#include "uart_comm.hpp"
#include <stdarg.h>
#include <stdio.h>


UartComm::UartComm(UART_HandleTypeDef *huart)
{
  this->huart = huart;
}

void UartComm::enable_it(void)
{
    __HAL_UART_ENABLE_IT(this->huart, UART_IT_PE);    // enable the UART Parity Error Interrupt
    __HAL_UART_ENABLE_IT(this->huart, UART_IT_ERR);   // enable the UART Error Interrupt: (Frame error, noise error, overrun error)
    __HAL_UART_ENABLE_IT(this->huart, UART_IT_RXNE);  // enable the UART Data Register not empty Interrupt
}

void UartComm::disable_it(void)
{
    __HAL_UART_DISABLE_IT(this->huart, UART_IT_RXNE);  // disable the UART Data Register not empty Interrupt
    __HAL_UART_DISABLE_IT(this->huart, UART_IT_PE);    // disable the UART Parity Error Interrupt
    __HAL_UART_DISABLE_IT(this->huart, UART_IT_ERR);   // disable the UART Error Interrupt: (Frame error, noise error, overrun error)
}

void UartComm::transmit(uint8_t data) { HAL_UART_Transmit(this->huart, &data, 1, TIMEOUT_TRANSMIT); }

void UartComm::transmit(uint8_t *data, uint16_t size) { HAL_UART_Transmit(this->huart, data, size, TIMEOUT_TRANSMIT); }

void UartComm::transmit(const char *data)
{
    uint8_t cnt = 0;
    for (; *(data + cnt); cnt++) { ; }
    transmit((uint8_t *)data, cnt);
}

void UartComm::printf(const char *format, ...)
{
    char    buf[256];
    int16_t cnt = 0;

    va_list list;
    va_start(list, format);
    cnt = vsprintf(buf, format, list);
    transmit((uint8_t *)buf, cnt);
    va_end(list);
}

int UartComm::receive(uint8_t *data)
{
    uint32_t isrflags   = READ_REG(this->huart->Instance->SR);
    uint32_t errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));

    if ((isrflags & USART_SR_RXNE) == RESET)
    {
        return -1;
    }
    else
    {
        *data = (uint8_t)get_dr();
        return errorflags;
    }
}

void UartComm::handle_irq(void){
  uint32_t isrflags   = READ_REG(this->huart->Instance->SR);
  uint32_t cr1its     = READ_REG(this->huart->Instance->CR1);
  uint32_t errorflags = 0x00U;

  /* check error flags */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));

  /* UART received data is enabled */
  if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
    uint16_t rx_data = get_dr();
    if (this->cb != nullptr) this->cb(rx_data, 1, errorflags);
  }

  return;
}
