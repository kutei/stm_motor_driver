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
  for(int i = 0; i <= argc; i++){
    fg_cmdlin_uart->printf("args %d: [%s]", i, argv[i]);
    fg_cmdlin_uart->transmit_linesep();
  }
}


/****************************************
 * メイン関数
 ****************************************/
void user_main(void){
  CmdlineUart cmdline_uart(&huart2);
  args_t args = {0};
  char** argv = std::array<char *, ARGS_NUM>().data();

  g_uart_comm = &cmdline_uart;
  fg_cmdlin_uart = &cmdline_uart;
  args_to_argv(argv, &args);

  cmdline_uart.transmit("\e[5B\e[2J\e[0;0H");  // 5行下、画面全クリア、カーソル位置(0,0)
  cmdline_uart.transmit("########################################\n\r");
  cmdline_uart.transmit("#     command line tools started!!     #\n\r");
  cmdline_uart.transmit("########################################\n\r");
  cmdline_uart.transmit("$ ");
  cmdline_uart.enable_it();

  while(1){
    if(cmdline_uart.is_execute_requested()){
      int argc = cmdline_uart.get_commands(&args);

      if(argc > 0){
        print_cmdline_args(argc, argv);
        cmdline_uart.transmit("----\n\r");
        for(int i = 0; i < 10; i++){
          cmdline_uart.printf("args %d: [%s]\n\r", i, argv[i]);
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
