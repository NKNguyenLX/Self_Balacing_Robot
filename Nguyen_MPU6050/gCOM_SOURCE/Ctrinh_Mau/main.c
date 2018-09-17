#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

void uart_init(unsigned int BAUD){ //khoi dong UART
	//set toc do baud------------------------------
	uint32_t temp_BAUD;
	temp_BAUD=F_CPU/16;
	temp_BAUD/=BAUD;
	temp_BAUD--;
	UBRRH=(unsigned char)(temp_BAUD>>8);
	UBRRL=(unsigned char)(temp_BAUD);		
	//set khung truyen va kich hoat bo nhan du lieu
	UCSRA=0x00;
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	UCSRB=(1<<TXEN);	
}
//chuong trinh con phat ky tu uart (ham co ban)
void uart_char_tx(unsigned char chr){
	if (chr == '\n')
		uart_char_tx('\r');
      while (bit_is_clear(UCSRA,UDRE)) {}; //cho den khi bit UDRE=1 
      UDR=chr;
}
//tao 1 stream ten mystdout thuoc loai FILE (FILE ao), connect voi ham uart_char_tx
static FILE uartstd= FDEV_SETUP_STREAM(uart_char_tx, NULL,_FDEV_SETUP_WRITE);

void uart_int_print(char ch, int val){ //goi data qua uart kem voi kenh du lieu
	uart_char_tx(ch+48);
	fprintf(&uartstd, "%i", val); //printf voi device duoc chi dinh
	uart_char_tx(' ');
}

int read_adc(unsigned char adc_channel){//chuong trinh con doc ADC theo tung channel	
    ADMUX=adc_channel|(1<<REFS1)|(1<<REFS0); //chon dien ap tham chieu noi 2.56V
    ADCSRA|=(1<<ADSC); 					//bat dau chuyen doi             
	loop_until_bit_is_set(ADCSRA,ADIF); //cho den khi nao bit ADIF==1  
    return ADCW;
}
int main(void){	
	uart_init(38400); //khoi dong uart
	
	//---khoi dong ADC-------------------------------------
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS0); 	//enable ADC, khong dung interrupt			
	int ADC_val;	
	while(1){
		ADC_val=read_adc(0);
		uart_int_print(1, ADC_val);	//goi chuong trinh send data		
		_delay_ms(5);
	}
}

