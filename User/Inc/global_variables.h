/*
 * global_variables.h
 *
 *  Created on: 2022/07/04
 *      Author: yumas
 */

#ifndef INC_GLOBAL_VARIABLES_H_
#define INC_GLOBAL_VARIABLES_H_

#include "stm32f1xx.h"
#include "uart_tools.hpp"
#include "tim1_encoder.hpp"
#include "gpo_tools.hpp"


// CubeMX defined variables
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


// user defined variables
extern bool g_kill_signal;
extern UartComm *g_uart_comm;
extern DoubleControlledPwm *g_pwm_output;
extern Tim1Encoder *g_tim1_encoder;
extern SbusUart *g_sbus_uart;

#endif /* INC_GLOBAL_VARIABLES_H_ */
