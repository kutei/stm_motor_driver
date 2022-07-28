/*
 * user_main.cpp
 *
 *  Created on: May 29, 2022
 *      Author: kutei
 */

#include "uart_tools.hpp"
#include "user_main.hpp"
#include "constants.hpp"
#include "global_variables.h"


extern UART_HandleTypeDef huart2;

void user_main(void){
  args_t args;
  CmdlineUart cmdline_uart(&huart2);
  g_uart_comm = &cmdline_uart;

  cmdline_uart.transmit_linesep();
  cmdline_uart.transmit("$ ");
  cmdline_uart.enable_it();

  while(1){
    if(cmdline_uart.is_execute_requested()){
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
      cmdline_uart.transmit("requested");
      cmdline_uart.transmit_linesep();
      cmdline_uart.get_commands(&args);
      cmdline_uart.printf("args 0: [%s]\n\r", args[0].data());
      cmdline_uart.printf("args 1: [%s]\n\r", args[1].data());
      cmdline_uart.printf("args 2: [%s]\n\r", args[2].data());
      cmdline_uart.printf("args 3: [%s]\n\r", args[3].data());
      cmdline_uart.printf("args 4: [%s]\n\r", args[4].data());
      cmdline_uart.printf("args 5: [%s]\n\r", args[5].data());
      cmdline_uart.printf("args 6: [%s]\n\r", args[6].data());
      cmdline_uart.printf("args 7: [%s]\n\r", args[7].data());
      cmdline_uart.printf("args 8: [%s]\n\r", args[8].data());
      cmdline_uart.printf("args 9: [%s]\n\r", args[9].data());
      cmdline_uart.transmit("$ ");
    }
    if(g_kill_signal){
      g_kill_signal = false;
      cmdline_uart.transmit_linesep();
      cmdline_uart.transmit("$ ");
      cmdline_uart.clear_queue();
    }
  }
}

void USER_USART2_IRQHandler(void){
  g_uart_comm->handle_irq();
}
