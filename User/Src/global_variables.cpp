/*
 * global_variables.c
 *
 *  Created on: 2022/07/04
 *      Author: yumas
 */

#include "global_variables.h"

bool g_kill_signal = false;
UartComm *g_uart_comm;
DoubleControlledPwm *g_pwm_output;
