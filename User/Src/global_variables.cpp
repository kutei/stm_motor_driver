/*
 * global_variables.c
 *
 *  Created on: 2022/07/04
 *      Author: yumas
 */

#include "global_variables.h"

bool g_kill_signal = false;
bool g_control_active = false;

uint32_t g_adc_current = 0;

UartComm *g_uart_comm;
DoubleControlledPwm *g_pwm_output;
Tim1Encoder *g_tim1_encoder;
SbusUart *g_sbus_uart;
PidController *g_pid_controller;
