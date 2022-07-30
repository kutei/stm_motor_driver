/*
 * global_variables.h
 *
 *  Created on: 2022/07/04
 *      Author: yumas
 */

#ifndef INC_GLOBAL_VARIABLES_H_
#define INC_GLOBAL_VARIABLES_H_

#include <uart_tools.hpp>

// CubeMX defined variables
extern UART_HandleTypeDef huart2;


// user defined variables
extern bool g_kill_signal;
extern UartComm *g_uart_comm;


#endif /* INC_GLOBAL_VARIABLES_H_ */
