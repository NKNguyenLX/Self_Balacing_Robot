#define USE_CLOCK
#define USE_I2C_USCI
#define USE_MPU6050
#define USE_UART




#include <math.h>
#include "msp430g2553.h"
#include "Global.h"
#include "UART.h"
#include "Clock.h"
#include "I2C_USCI.h"
#include "MPU6050.h"




char buff[50];

//Chu y Gyro bi sai so tinh khong dong nhat khi goc quay thay doi
// Khong tinh duoc vi phan cua gyro

void main()
{
	WDTCTL = WDTPW | WDTHOLD;
        Select_Clock(DCO_16MHZ);
	__delay_cycles(100000);   
        P1DIR = BIT0;	
        
        Init_I2C_USCI(MPU6050_ADDRESS); //1.7 SDA, 1.6 SCL
	Setup_MPU6050();    
	__delay_cycles(320000);       
       
	
        UART_Init();	
	while(1)
        {                    
                Get_Accel_Values(); 
                Get_Gyro_Values(); 
               //Get_Angle_filter();
               // __delay_cycles(1000000);
              
                              
                UART_Write_Char('1');
                UART_Write_Float(ACCEL_XOUT-400,0);
                //UART_Write_Float(GYRO_XOUT+400,0);
                UART_Write_Char(' ');               
                
                UART_Write_Char('2');
                UART_Write_Float(ACCEL_YOUT+160,0);
                //UART_Write_Float(GYRO_YOUT-90,0);
                UART_Write_Char(' ');                               
                
                UART_Write_Char('3');
                UART_Write_Float(ACCEL_ZOUT-400,0);
                //UART_Write_Float(GYRO_ZOUT+60,0);
                UART_Write_Char(' ');
                
		
                
                P1OUT ^= BIT0;
        }
                
	
}


