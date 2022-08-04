/*
 * uart_comm.cpp
 *
 *  Created on: Jun 4, 2022
 *      Author: yumas
 */

#include "global_variables.h"
#include "uart_tools.hpp"

#include <stdarg.h>
#include <stdio.h>


UartComm::UartComm(UART_HandleTypeDef *huart)
{
    this->huart = huart;
}

void UartComm::enable_it(void)
{
    __HAL_UART_ENABLE_IT(this->huart, UART_IT_PE);    // enable the UART Parity Error Interrupt
    __HAL_UART_ENABLE_IT(this->huart, UART_IT_ERR);   // enable the UART Error Interrupt: (Frame error, noise error, overrun error)
    __HAL_UART_ENABLE_IT(this->huart, UART_IT_RXNE);  // enable the UART Data Register not empty Interrupt
}

void UartComm::disable_it(void)
{
    __HAL_UART_DISABLE_IT(this->huart, UART_IT_RXNE);  // disable the UART Data Register not empty Interrupt
    __HAL_UART_DISABLE_IT(this->huart, UART_IT_PE);    // disable the UART Parity Error Interrupt
    __HAL_UART_DISABLE_IT(this->huart, UART_IT_ERR);   // disable the UART Error Interrupt: (Frame error, noise error, overrun error)
}

void UartComm::transmit(uint8_t data){ HAL_UART_Transmit(this->huart, &data, 1, TIMEOUT_TRANSMIT); }

void UartComm::transmit(uint8_t *data, uint16_t size){ HAL_UART_Transmit(this->huart, data, size, TIMEOUT_TRANSMIT); }

void UartComm::transmit(const char *data)
{
    uint8_t cnt = 0;
    for(; *(data + cnt); cnt++){ ; }
    transmit((uint8_t *)data, cnt);
}

void UartComm::transmit_linesep() { this->transmit("\n\r"); }

void UartComm::printf(const char *format, ...)
{
    char buf[256];
    int16_t cnt = 0;

    va_list list;
    va_start(list, format);
    cnt = vsprintf(buf, format, list);
    transmit((uint8_t *)buf, cnt);
    va_end(list);
}

int UartComm::receive(uint16_t *data)
{
    uint32_t isrflags = READ_REG(this->huart->Instance->SR);
    uint32_t errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));

    if((isrflags & USART_SR_RXNE) == RESET){
        return -1;
    }else{
        *data = get_dr();
        return errorflags;
    }
}

void UartComm::handle_irq(void)
{
    uint32_t isrflags   = READ_REG(this->huart->Instance->SR);
    uint32_t cr1its     = READ_REG(this->huart->Instance->CR1);
    uint32_t errorflags = 0x00U;

    /* check error flags */
    errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));

    /* UART received data is enabled */
    if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)){
        uint16_t rx_data = get_dr();
        this->callback(rx_data, 1, errorflags);
    }

    return;
}


void QueuedUart::callback(uint16_t data, size_t len, uint32_t error_flag)
{
    this->queue.emplace_back(data);
}


void CmdlineUart::callback(uint16_t data, size_t len, uint32_t error_flag)
{
    if(data == 3){
        this->transmit("^C");
        g_kill_signal = true;
    }else if(data == 8){
        if(this->is_echo && this->queue.size() > 0){
            this->queue.pop_back();
            this->transmit(8);
            this->transmit(' ');
            this->transmit(8);
        }
    }else if((' ' <= data && data <= '~') || data == '\r'){
        uint8_t data_uint8 = static_cast<uint8_t>(data);
        this->queue.emplace_back(data_uint8);
        if(this->is_echo){
            this->transmit(data_uint8);
            if(data_uint8 == '\r') this->transmit("\n");
        }
    }else if(data == '\e'){
        uint8_t data_uint8 = '^';
        this->queue.emplace_back(data_uint8);
        if(this->is_echo){
            this->transmit(data_uint8);
            if(data_uint8 == '\r') this->transmit("\n");
        }
    }
}

void CmdlineUart::clear_queue(void){
    this->queue.clear();
}

bool CmdlineUart::is_execute_requested(void){
    size_t queue_size = this->queue.size();
    for(size_t i = 0; i < queue_size; i++){
        if(this->queue[i] == '\r') return true;
    }
    return false;
}

int CmdlineUart::get_commands(args_t *args){
    for(size_t i = 0; i < ARGS_NUM; i++){
        for(size_t j = 0; j < ARG_LENGTH; j++){
            (*args)[i][j] = 0;
        }
    }

    size_t n_args = 0;
    size_t n_chars = 0;
    while(true){
        uint8_t data = this->queue.front();
        this->queue.pop_front();

        if(data == '\r'){
            if(n_args == 0 && n_chars == 0) return 0;
            if(n_args >= ARGS_NUM) return ARGS_NUM;
            if(n_chars > 0) return n_args + 1;
            return n_args;
        }

        if(n_chars > 0 && data == ' '){
            n_chars = 0;
            n_args++;
        }
        if('!' <= data && data <= '~'){
            if(n_args < ARGS_NUM && n_chars < (ARG_LENGTH-1)){
                (*args)[n_args][n_chars] = data;
            }
            n_chars++;
        }
    }
}


SbusUart::SbusUart(UART_HandleTypeDef *huart) : UartComm(huart){
    for(int i = 0; i < 18; i++) this->channels[i] = 0;
}

int16_t SbusUart::get_channel(int channel){
    if (channel < 1 or channel > 18) {
        return 0;
    } else {
        return this->channels[channel - 1] - 1024;
    }
}
uint16_t SbusUart::get_decoder_err(){
    return this->decoderErrorFrames;
}
int16_t SbusUart::get_fail_safe(){
    return this->failsafe;
}
int16_t SbusUart::get_lost_frames(){
    return this->lostFrames;
}
uint16_t SbusUart::get_error_rate(){
    return this->error_rate;
}
bool SbusUart::is_active(){
    if(this->error_rate < this->ERR_STOP){
        return true;
    }else{
        return false;
    }
}
void SbusUart::error_reset(){
    this->error_rate = 0;
}
void SbusUart::error(){
    if(this->error_rate < this->ERR_CRITICAL){
        this->error_rate++;
    }
}
void SbusUart::callback(uint16_t data, size_t len, uint32_t error_flag){
    if(this->buf_count == 0 && data != STARTBYTE){
        // incorrect start byte, out of sync
        this->decoderErrorFrames++;
        this->error();
        return;
    }

    this->buffer[this->buf_count] = data;
    this->buf_count++;

    if(this->buf_count == 25){
        this->buf_count = 0;

        if(this->buffer[23] != this->ENDBYTE){
            this->decoderErrorFrames++;
            this->error();
            return;
        }

        this->channels[0]  = ((this->buffer[1]    |this->buffer[2]<<8)                 & 0x07FF);
        this->channels[1]  = ((this->buffer[2]>>3 |this->buffer[3]<<5)                 & 0x07FF);
        this->channels[2]  = ((this->buffer[3]>>6 |this->buffer[4]<<2 |this->buffer[5]<<10)  & 0x07FF);
        this->channels[3]  = ((this->buffer[5]>>1 |this->buffer[6]<<7)                 & 0x07FF);
        this->channels[4]  = ((this->buffer[6]>>4 |this->buffer[7]<<4)                 & 0x07FF);
        this->channels[5]  = ((this->buffer[7]>>7 |this->buffer[8]<<1 |this->buffer[9]<<9)   & 0x07FF);
        this->channels[6]  = ((this->buffer[9]>>2 |this->buffer[10]<<6)                & 0x07FF);
        this->channels[7]  = ((this->buffer[10]>>5|this->buffer[11]<<3)                & 0x07FF);
        this->channels[8]  = ((this->buffer[12]   |this->buffer[13]<<8)                & 0x07FF);
        this->channels[9]  = ((this->buffer[13]>>3|this->buffer[14]<<5)                & 0x07FF);
        this->channels[10] = ((this->buffer[14]>>6|this->buffer[15]<<2|this->buffer[16]<<10) & 0x07FF);
        this->channels[11] = ((this->buffer[16]>>1|this->buffer[17]<<7)                & 0x07FF);
        this->channels[12] = ((this->buffer[17]>>4|this->buffer[18]<<4)                & 0x07FF);
        this->channels[13] = ((this->buffer[18]>>7|this->buffer[19]<<1|this->buffer[20]<<9)  & 0x07FF);
        this->channels[14] = ((this->buffer[20]>>2|this->buffer[21]<<6)                & 0x07FF);
        this->channels[15] = ((this->buffer[21]>>5|this->buffer[22]<<3)                & 0x07FF);

        ((this->buffer[23])      & 0x0001) ? this->channels[16] = 2047: this->channels[16] = 0;
        ((this->buffer[23] >> 1) & 0x0001) ? this->channels[17] = 2047: this->channels[17] = 0;

        if((this->buffer[23] >> 3) & 0x0001){
            this->failsafe = this->FAILSAFE_ACTIVE;
        }else{
            this->failsafe = this->FAILSAFE_INACTIVE;
        }

        if ((this->buffer[23] >> 2) & 0x0001) {
            this->lostFrames++;
        }

        this->error_reset();

    }
}