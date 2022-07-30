/*
 * uart_comm.cpp
 *
 *  Created on: Jun 4, 2022
 *      Author: yumas
 */

#include "global_variables.h"
#include "uart_tools.hpp"

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

void UartComm::transmit_linesep() { this->transmit("\n\r"); }

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
    this->callback(rx_data, 1, errorflags);
  }

  return;
}


void QueuedUart::callback(uint16_t data, size_t len, uint32_t error_flag){
  this->queue.emplace_back(data);
}


void CmdlineUart::callback(uint16_t data, size_t len, uint32_t error_flag){
  if(data == 3){
    this->transmit("^C");
    g_kill_signal = true;
  }else if(data == 8){
    if(this->is_echo && this->queue.size() > 0){
      this->queue.pop_back();
      this->transmit(8);
      this->transmit(' ');
      this->transmit(8);
    }
  }else if((' ' <= data && data <= '~') || data == '\r'){
    uint8_t data_uint8 = static_cast<uint8_t>(data);
    this->queue.emplace_back(data_uint8);
    if(this->is_echo) {
      this->transmit(data_uint8);
      if(data_uint8 == '\r') this->transmit("\n");
    }
  }else if(data == '\e'){
    uint8_t data_uint8 = '^';
    this->queue.emplace_back(data_uint8);
    if(this->is_echo) {
      this->transmit(data_uint8);
      if(data_uint8 == '\r') this->transmit("\n");
    }
  }
}

void CmdlineUart::clear_queue(void){
  this->queue.clear();
}

bool CmdlineUart::is_execute_requested(void){
  size_t queue_size = this->queue.size();
  for(size_t i = 0; i < queue_size; i++){
    if(this->queue[i] == '\r') return true;
  }
  return false;
}

int CmdlineUart::get_commands(args_t *args){
  for(size_t i = 0; i < ARGS_NUM; i++){
    for(size_t j = 0; j < ARG_LENGTH; j++){
      (*args)[i][j] = 0;
    }
  }

  size_t n_args = 0;
  size_t n_chars = 0;
  while(true){
    uint8_t data = this->queue.front();
    this->queue.pop_front();
    if(data == '\r'){
      if(n_args > ARGS_NUM){ return ARGS_NUM-1; }
      return n_args;
    }

    if(n_chars > 0 && data == ' '){
      n_chars = 0;
      n_args++;
    }
    if('!' <= data && data <= '~'){
      if(n_args < ARGS_NUM && n_chars < (ARG_LENGTH-1)){
        (*args)[n_args][n_chars] = data;
      }
      n_chars++;
    }
  }
}
