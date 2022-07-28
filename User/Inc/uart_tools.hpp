/*
 * uart_comm.hpp
 *
 *  Created on: Jun 4, 2022
 *      Author: yumas
 */

#ifndef INC_UART_TOOLS_HPP_
#define INC_UART_TOOLS_HPP_

#include "constants.hpp"
#include "stm32f1xx.h"
#include <queue>

#ifdef __cplusplus
extern "C" {
#endif

class UartComm{
public:
  static constexpr int TIMEOUT_TRANSMIT = 10;
  typedef void (*uart_callback_t)(uint16_t, size_t, uint32_t);


  UartComm(UART_HandleTypeDef *huart);

  /**
   * @brief 割り込みを有効にする。
   *
   * @retval None
   */
  void enable_it(void);

  /**
   * @brief 割り込みを無効にする。
   *
   * @retval None
   */
  void disable_it(void);

  /**
   * @brief データを送信
   *
   * @param[in] data 送信するデータ
   * @retval None
   */
  void transmit(uint8_t data);

  /**
   * @brief データを送信
   *
   * @param[in] data 送信するデータの先頭ポインタ
   * @param[in] size 送信するデータのデータ長
   * @retval None
   */
  void transmit(uint8_t *data, uint16_t size);

  /**
   * @brief 文字列データを送信
   *
   * @param[in] data 送信する文字列
   * @retval None
   */
  void transmit(const char *data);

  /**
   * @brief 改行コードを送信
   *
   * @retval None
   */
  void transmit_linesep();

  /**
   * @brief フォーマット文字列を送信
   *
   * @param[in] format 送信するformat指定子を含む文字列
   * @param[in] ... 可変長引数
   * @retval None
   */
  void printf(const char *format, ...);

  /**
   * @brief 文字列データを受信（コールバックが有効の場合は使用できない）
   *
   * @param[in] data 送信する文字列
   * @retval 0 受信できるデータが存在し、引数dataに値が設定された。
   * @retval 1以上 受信できるデータが存在し、引数dataに値が設定されたが、受信時にエラーが発生した。
   * @retval -1 受信できるデータが存在しない。
   */
  int receive(uint8_t *data);

  /**
   * @brief 割り込み要求ハンドラを処理する。
   *
   * @retval None
   */
  void handle_irq(void);

private:
  UART_HandleTypeDef* huart;
  uart_callback_t     cb = nullptr;

  /**
   * @brief USART_DRから1Byte(8,9bits)のデータを取得する。
   *
   * @note 各種レジスタの値を参照し、USART_DRからデータ取得可能なことを確認してから呼び出すこと。
   * @retval None
   */
  inline uint16_t get_dr(void){ return (uint16_t)(huart->Instance->DR & (uint32_t)0x00FF); }

  /**
   * @brief 受信コールバック関数
   *
   * @param[in] data 受信データ
   * @param[in] len 受信長
   * @param[in] error_flag エラーフラグ
   * @retval None
   */
  virtual void callback(uint16_t data, size_t len, uint32_t error_flag) = 0;
};


class QueuedUart : public UartComm{
public:
  std::deque<uint16_t> queue;

  QueuedUart(UART_HandleTypeDef *huart) : UartComm(huart) { ; }

private:

  /**
   * @brief 受信コールバック関数
   *
   * @param[in] data 受信データ
   * @param[in] len 受信長
   * @param[in] error_flag エラーフラグ
   * @retval None
   */
  void callback(uint16_t data, size_t len, uint32_t error_flag);
};


class CmdlineUart : public QueuedUart{
public:
  CmdlineUart(UART_HandleTypeDef *huart) : QueuedUart(huart) { ; }
  void enable_echo(void) { is_echo = true; }
  void disable_echo(void) { is_echo = false; }
  void clear_queue(void);
  bool is_execute_requested(void);
  void get_commands(args_t *args);

private:
  bool is_echo = true;

  /**
   * @brief 受信コールバック関数
   *
   * @param[in] data 受信データ
   * @param[in] len 受信長
   * @param[in] error_flag エラーフラグ
   * @retval None
   */
  void callback(uint16_t data, size_t len, uint32_t error_flag);
};

#ifdef __cplusplus
}
#endif

#endif /* INC_UART_TOOLS_HPP_ */
