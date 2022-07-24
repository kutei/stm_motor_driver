/*
 * user_main.cpp
 *
 *  Created on: May 29, 2022
 *      Author: kutei
 */

#include "user_main.hpp"
#include "uart_comm.hpp"
#include "global_variables.h"


extern UART_HandleTypeDef huart2;

void user_main(void){

  g_uart_comm = new UartComm(&huart2);
  g_uart_comm->transmit("\n\r$ ");
  g_uart_comm->enable_it();

  while(1){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
  }
}

void USER_USART2_IRQHandler(void){
  g_uart_comm->handle_irq();
}
