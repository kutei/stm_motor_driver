/*
 * user_it.cpp
 *
 *  Created on: 2022/07/30
 *      Author: yumas
 */

#include "user_it.hpp"
#include "global_variables.h"

void USER_USART1_IRQHandler(void){
    g_sbus_uart->handle_irq();
}

void USER_USART2_IRQHandler(void){
    g_uart_comm->handle_irq();
}

void USER_TIM4_IRQHandler(void){
    g_pid_controller->callback();
}
