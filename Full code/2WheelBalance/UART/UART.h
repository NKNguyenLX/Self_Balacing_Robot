/*
 * UART.h
 *
 *  Created on: Aug 28, 2017
 *      Author: Dang Thanh Tung
 */

#ifndef UART_UART_H_
#define UART_UART_H_

#define SYSCTL_PERIPH_UART          SYSCTL_PERIPH_UART0
#define SYSCTL_PERIPH_GPIO          SYSCTL_PERIPH_GPIOA
#define GPIO_RX                     GPIO_PA0_U0RX
#define GPIO_TX                     GPIO_PA1_U0TX
#define GPIO_PORT_BASE              GPIO_PORTA_BASE
#define UART_BASE                   UART0_BASE
#define INT_UART                    INT_UART0

void Uart_Init ();
void Bluetooth_RxTxHandler(void);
uint16_t bluetooth_recv(uint8_t* rxBuf, uint16_t numToRead, bool block);
static uint16_t ReadRxBuffer(uint8_t* rxBuf, uint16_t numToRead);
bool bluetooth_send(const uint8_t *pui8Buffer, uint32_t ui32Count);
#endif /* UART_UART_H_ */
