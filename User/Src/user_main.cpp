/*
 * user_main.cpp
 *
 *  Created on: May 29, 2022
 *      Author: kutei
 */

#include "uart_tools.hpp"
#include "gpo_tools.hpp"
#include "user_main.hpp"
#include "constants.hpp"
#include "global_variables.h"
#include <string.h>
#include <map>


static CmdlineUart *fg_cmdlin_uart;


/****************************************
 * Util関数郡
 ****************************************/
void args_to_argv(char** argv, args_t* args){
    for(size_t i = 0; i < ARGS_NUM; i++){
        argv[i] = (*args)[i].data();
    }
}


/****************************************
 * コマンド実行関数郡
 ****************************************/
void print_cmdline_args(int argc, char** argv){
    for(int i = 0; i < argc; i++){
        fg_cmdlin_uart->printf("args %d: [%s]", i, argv[i]);
        fg_cmdlin_uart->transmit_linesep();
    }
}


/****************************************
 * メイン関数
 ****************************************/
void user_main(void){
    args_t args = {0};
    char** argv = std::array<char *, ARGS_NUM>().data();

    typedef void (*cmd_func_t)(int, char**);
    std::map<const char *, cmd_func_t> cmd_list{
        {"echo", print_cmdline_args}
    };

    CmdlineUart cmdline_uart(&huart2);
    DoubleControlledPwm pwm_output(&htim3, false);

    g_uart_comm = &cmdline_uart;
    fg_cmdlin_uart = &cmdline_uart;
    args_to_argv(argv, &args);

    cmdline_uart.transmit("\e[5B\e[2J\e[0;0H");  // 5行下、画面全クリア、カーソル位置(0,0)
    cmdline_uart.transmit("########################################\n\r");
    cmdline_uart.transmit("#     command line tools started!!     #\n\r");
    cmdline_uart.transmit("########################################\n\r");
    cmdline_uart.transmit("$ ");
    cmdline_uart.enable_it();

    led_red_set();
    led_blue_set();

    pwm_output.start();
    pwm_output.set(3000);

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
