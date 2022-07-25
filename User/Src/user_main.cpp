/*
 * user_main.cpp
 *
 *  Created on: May 29, 2022
 *      Author: kutei
 */

#include <uart_tools.hpp>
#include "user_main.hpp"
#include "global_variables.h"


extern UART_HandleTypeDef huart2;

void user_main(void){
  QueuedUart queued_uart(&huart2);
  g_uart_comm = &queued_uart;


  queued_uart.transmit("\n\r$ ");
  queued_uart.enable_it();

  while(1){
    if(queued_uart.queue.size() > 0){
      uint8_t rec = static_cast<uint8_t>(queued_uart.queue.front());
      queued_uart.queue.pop();
      queued_uart.transmit(rec);
    }
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
  }
}

void USER_USART2_IRQHandler(void){
  g_uart_comm->handle_irq();
}
