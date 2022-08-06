/*
 * user_main.cpp
 *
 *  Created on: May 29, 2022
 *      Author: kutei
 */

#include "uart_tools.hpp"
#include "gpo_tools.hpp"
#include "tim1_encoder.hpp"
#include "user_main.hpp"
#include "constants.hpp"
#include "global_variables.h"
#include <string.h>
#include <map>
#include <stdlib.h>

static CmdlineUart *fg_cmdlin_uart;


/****************************************
 * Util関数郡
 ****************************************/
void args_to_argv(char** argv, args_t* args){
    for(size_t i = 0; i < ARGS_NUM; i++){
        argv[i] = (*args)[i].data();
    }
}

void enable_output(void){
    led_red_reset();
    led_blue_set();
    g_pwm_output->set(0);
    pwm_output_enable();
}

void disable_output(void){
    pwm_output_disable();
    g_pwm_output->set(0);
    led_red_set();
    led_blue_reset();
}

/****************************************
 * コマンド実行関数郡
 ****************************************/
void cmd_print_cmdline_args(int argc, char** argv){
    for(int i = 0; i < argc; i++){
        fg_cmdlin_uart->printf("args %d: [%s]", i, argv[i]);
        fg_cmdlin_uart->transmit_linesep();
    }
}

void rotation_triangle(int max){
    uint32_t sleep_time = 500;

    if(max > DoubleControlledPwm::MAX_PERIOD){ max = DoubleControlledPwm::MAX_PERIOD; }
    if(max < 0){ max = 0; }

    float div_max = static_cast<float>(max) / DoubleControlledPwm::MAX_PERIOD;

    fg_cmdlin_uart->transmit("start foward rotation");
    for(int i = 0; i < DoubleControlledPwm::MAX_PERIOD; i++){
        if(g_kill_signal) return;
        g_pwm_output->set(i * div_max);
        for(uint32_t j = 0; j < sleep_time; j++) asm("NOP");
    }
    fg_cmdlin_uart->transmit(" - TOP");
    fg_cmdlin_uart->transmit_linesep();
    for(int i = DoubleControlledPwm::MAX_PERIOD; i > 0; i--){
        if(g_kill_signal) return;
        g_pwm_output->set(i * div_max);
        for(uint32_t j = 0; j < sleep_time; j++) asm("NOP");
    }
    fg_cmdlin_uart->transmit("start backward rotation");
    for(int i = 0; i < DoubleControlledPwm::MAX_PERIOD; i++){
        if(g_kill_signal) return;
        g_pwm_output->set(-i * div_max);
        for(uint32_t j = 0; j < sleep_time; j++) asm("NOP");
    }
    fg_cmdlin_uart->transmit(" - TOP");
    fg_cmdlin_uart->transmit_linesep();
    for(int i = DoubleControlledPwm::MAX_PERIOD; i > 0; i--){
        if(g_kill_signal) return;
        g_pwm_output->set(-i * div_max);
        for(uint32_t j = 0; j < sleep_time; j++) asm("NOP");
    }
    fg_cmdlin_uart->transmit("finish");
    fg_cmdlin_uart->transmit_linesep();
}
void cmd_triangle(int argc, char** argv){
    if(argc == 1){
        fg_cmdlin_uart->transmit("error: one argument is needed. usage: triangle MAX");
        fg_cmdlin_uart->transmit_linesep();
        return;
    }
    int max = atoi(argv[1]);

    enable_output();

    rotation_triangle(max);

    disable_output();

    g_kill_signal = false;
}

void cmd_display_encoder(int argc, char** argv){
    while(1){
        if(g_kill_signal){
            g_kill_signal = false;
            return;
        }

        fg_cmdlin_uart->printf(
            "deg:%d, CNT:%d, spd:%d",
            g_tim1_encoder->get_current(), TIM1->CNT, g_tim1_encoder->get_speed());
        fg_cmdlin_uart->transmit_linesep();
        HAL_Delay(100);
    }
}

void cmd_display_sbus(int argc, char** argv){
    while(1){
        if(g_kill_signal){
            g_kill_signal = false;
            return;
        }

        fg_cmdlin_uart->printf("1:%5d, 2:%5d, 3:%5d, 4:%5d, 5:%5d, 6:%5d, err:%4d, ErrRate:%4d",
                g_sbus_uart->get_channel(1),
                g_sbus_uart->get_channel(2),
                g_sbus_uart->get_channel(3),
                g_sbus_uart->get_channel(4),
                g_sbus_uart->get_channel(5),
                g_sbus_uart->get_channel(6),
                g_sbus_uart->get_decoder_err(),
                g_sbus_uart->get_error_rate()
        );
        if(g_sbus_uart->get_fail_safe() == SbusUart::FAILSAFE_ACTIVE){
            fg_cmdlin_uart->transmit(", FS:ACTIVE");
        }else{
            fg_cmdlin_uart->transmit(", FS:INACTIVE");
        }
        fg_cmdlin_uart->transmit_linesep();
        HAL_Delay(100);
    }
}

void cmd_display_pid(int argc, char** argv){
    while(1){
        if(g_kill_signal){
            g_kill_signal = false;
            return;
        }

        g_pid_controller->display();
        fg_cmdlin_uart->transmit_linesep();
        HAL_Delay(100);
    }
}

void cmd_display_pid_full(int argc, char** argv){
    while(1){
        if(g_kill_signal){
            g_kill_signal = false;
            return;
        }

        g_pid_controller->display_full();
        fg_cmdlin_uart->transmit_linesep();
        HAL_Delay(100);
    }
}

void cmd_activate(int argc, char** argv){
    enable_output();
    g_control_active = true;
}
void cmd_stop(int argc, char** argv){
    disable_output();
    g_control_active = false;
}


/****************************************
 * メイン関数
 ****************************************/
void user_main(void){
    args_t args = {0};
    char** argv = std::array<char *, ARGS_NUM>().data();

    typedef void (*cmd_func_t)(int, char**);
    std::map<const char *, cmd_func_t> cmd_list{
        {"echo", cmd_print_cmdline_args},
        {"o_tri", cmd_triangle},
        {"enc", cmd_display_encoder},
        {"sbus", cmd_display_sbus},
        {"pid", cmd_display_pid},
        {"pid_full", cmd_display_pid_full},
        {"activate", cmd_activate},
        {"stop", cmd_stop}
    };

    CmdlineUart cmdline_uart(&huart2);
    SbusUart sbus_uart(&huart1);
    DoubleControlledPwm pwm_output(&htim3, false, 10000);
    Tim1Encoder tim1_encoder(&htim1);
    PidController pid_controller(14.5f, 0.9f, 0.2f, 0.005f, 10, 53000, -26000);

    g_uart_comm = &cmdline_uart;
    g_sbus_uart = &sbus_uart;
    fg_cmdlin_uart = &cmdline_uart;
    g_pwm_output = &pwm_output;
    g_tim1_encoder = &tim1_encoder;
    g_pid_controller = &pid_controller;
    args_to_argv(argv, &args);

    cmdline_uart.transmit("\e[5B\e[2J\e[0;0H");  // 5行下、画面全クリア、カーソル位置(0,0)
    cmdline_uart.transmit("########################################\n\r");
    cmdline_uart.transmit("#     command line tools started!!     #\n\r");
    cmdline_uart.transmit("########################################\n\r");
    cmdline_uart.transmit("$ ");
    cmdline_uart.enable_it();

    // 出力を停止
    disable_output();

    // 各機能を初期化
    sbus_uart.enable_it();
    g_tim1_encoder->start();
    pwm_output.start();

    // 制御割り込みの開始
    HAL_TIM_Base_Start_IT(&htim4);

    while(1){
        if(cmdline_uart.is_execute_requested()){
            int argc = cmdline_uart.get_commands(&args);

            if(argc > 0){
                bool executed = false;
                for(auto it = cmd_list.cbegin(); it != cmd_list.cend() ; it++){
                    if(strcmp(argv[0], it->first) == 0){
                        it->second(argc, argv);
                        executed = true;
                        break;
                    }
                }
                if(executed == false){
                    cmdline_uart.printf("error: unknown cmd for %s\n\r", argv[0]);
                }
            }
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
