/*
 * UART.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Dang Thanh Tung
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "UART.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.c"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#define MAX_RX_BUF              1500
#define MAX_TX_BUF              1500
static volatile uint8_t rxBuffer[MAX_RX_BUF];
static volatile uint16_t rxHead;
static uint16_t rxTail;
static uint16_t ui16_rxSize = 0;
//static bool b_is_has_new_data = false;

static volatile uint8_t txBuffer[MAX_TX_BUF];
static volatile uint16_t txHead;
static uint16_t txTail;

static uint16_t ReadRxBuffer(uint8_t* rxBuf, uint16_t numToRead);
void Bluetooth_RxTxHandler(void);

void Uart_Init (uint32_t baudrate)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO);
    GPIOPinConfigure(GPIO_RX);
    GPIOPinConfigure(GPIO_TX);
    GPIOPinTypeUART(GPIO_PORT_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), baudrate,
                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                UART_CONFIG_PAR_NONE));

    IntMasterEnable(); //enable processor interrupts
    UARTIntRegister(UART_BASE, &Bluetooth_RxTxHandler);
    IntEnable(INT_UART);
    UARTIntEnable(UART_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);  //bat ngat rx,tx,Receive Timeout
    UARTTxIntModeSet(UART_BASE, UART_TXINT_MODE_EOT);   //set mode ngat tx,
//    HC05_ClearEvtQueue();

}

void Bluetooth_RxTxHandler(void)        //chuong trinh ngat uart
{
    uint32_t IntStatus;
    IntStatus = UARTIntStatus(UART0_BASE, true);    //Lay trang thai ngat hien tai, tra ve bit mat na
    UARTIntClear(UART0_BASE, IntStatus);        //xoa co ngat

    if (IntStatus & UART_INT_TX)    //neu nhan dc ki tu de goi, vao ngat
    {
        if (txTail < MAX_TX_BUF)    //chi so fifo da goi
        {
            if (txHead != txTail)   //chi so fifo da dua vao txfifo khac da goi
            {
                UARTCharPutNonBlocking(UART0_BASE, txBuffer[txTail++]); //truyen vao txfifo cho goi di
            }
            else
            {
//                HC05_PutEvtIntoQueue(HC05_TX_DONE_EVENT);           // 2 cai bang nhau tuc la da goi het
            }
        }
        else            //chi so fifo da truyen di = max (0->max-1)
        {
            if (0 != txHead)            //chi so dua vao fifo khac 0 (da co du lieu vao fifo)
            {
                UARTCharPutNonBlocking(UART0_BASE, txBuffer[0]);    //cho goi di phan tu 0
                txTail = 1;     //chi so da goi = 1
            }
            else
            {
//                HC05_PutEvtIntoQueue(HC05_TX_DONE_EVENT);       //chi so dua vao bang 0, khong co du lieu vao, bao da goi het
            }
        }
    }
    while (UARTCharsAvail(UART0_BASE))      //neu co du lieu trong rxfifo tra ve 1
    {
//        b_is_has_new_data = true;           //da co data= true
        if (rxHead + 1 < MAX_RX_BUF)         //chi so du lieu lay ra +1 chua max (0-> max-1)
        {
            if ((rxHead + 1) != rxTail)     //chi so lay ra +1 khac chi so da nhan (con du lieu trong rxfifo)
            {
                rxBuffer[rxHead++] = UARTCharGet(UART0_BASE);   //lay du lieu tu rxfifo ra mang
                ui16_rxSize++;
            }
        }
        else        //neu chi so lay du lieu ra = max - 1 (0->max-1)
        {
            if (0 != rxTail)        //neu chi so du lieu da nhan vao fifo khac 0 (co du lieu)
            {
                rxBuffer[rxHead] = UARTCharGet(UART0_BASE); //lay max-1 ra
                ui16_rxSize++;
                rxHead = 0;     //reset ve 0
            }
        }
    }
}

uint16_t bluetooth_recv(uint8_t* rxBuf, uint16_t numToRead, bool block)
{
    if (block)          //cho cho den khi nhan du so byte yeu cau
    {
        // Blocking mode, so don't return until we read all the bytes requested
        uint16_t bytesRead;
        // Keep getting data if we have a number of bytes to fetch
        while (numToRead)
        {
            bytesRead = ReadRxBuffer(rxBuf, numToRead);
            if (bytesRead)
            {
                rxBuf += bytesRead;
                numToRead -= bytesRead;
            }
        }
        return bytesRead;
    } else {
        // Non-blocking mode, just read what is available in buffer
        return ReadRxBuffer(rxBuf, numToRead);
    }
}

static uint16_t ReadRxBuffer(uint8_t* rxBuf, uint16_t numToRead)
{
    uint16_t bytesRead = 0;

    while (rxTail != rxHead && bytesRead < numToRead)       //chi so lay ra khac chi so mang, bang nhau het data
    {
        if (rxTail + 1 < MAX_RX_BUF)        //chi so lay ra +1 nho hon max (0->max-1)
        {
            *rxBuf++ = rxBuffer[rxTail++];      //lay du lieu ra tu rxbuffer
            bytesRead++;                        //dem so byte
        }
        else            //chi so lay ra = max
        {
            *rxBuf++ = rxBuffer[rxTail];        //lay du lieu ra
            rxTail = 0;             //reset ve 0
            bytesRead++;
        }
    }

    return bytesRead;
}

bool bluetooth_send(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    if (txHead == txTail)       //chi so mang bang chi so da goi di
    {
        UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
        ui32Count--;
    }
    //
    // Loop while there are more characters to send.
    //
    while (ui32Count--)         //giam so ki tu goi
    {
        if (txHead + 1 < MAX_TX_BUF)    //chi so mang+1 < max (0->max-1)
        {
            if ((txHead + 1) != txTail)     //chi so mang +1 khac chi so goi
            {
                txBuffer[txHead++] = *pui8Buffer++;     //truyen vao mang
            } else
                return false;
        }
        else        //chi so mang = max
        {
            if (0 != txTail)        //chi so da goi khac 0, de co the truyen tiep
            {
                txBuffer[txHead] = *pui8Buffer++;   //goi gia tri max di
                txHead = 0;     //xoa ve 0
            } else
                return false;
        }
        //
        // Write the next character to the UART.
        //
//        UARTCharPut(UART0_BASE, *pui8Buffer++);
    }
    return true;
}

