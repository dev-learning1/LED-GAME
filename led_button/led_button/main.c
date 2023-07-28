/*
 * led_button.c
 *
 * Created: 2021-07-08 오후 8:51:24
 * Author : heejeong
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#define F_CPU 16000000L
#include <util/delay.h>



unsigned char ran;
volatile int cnt=0;
volatile int num=0;
volatile int flag=0;
volatile int level = 1;

void UART1_init(void)
{
	UCSR1A |= (1 << U2X1);				// 2배속 모드
	UBRR1H = 0x00;
	UBRR1L = 207;					// 보율 9600
	UCSR1C |= (1 << UCSZ11) | (1 << UCSZ10);	// 데이터비트수 8비트로 설정
	UCSR1B |= (1 << RXEN1)  | (1 << TXEN1);		// 송수신 활성화
}
void UART1_print_string(char *str)			// 문자열 송신 함수
{
	for(int i=0; str[i];i++)			// '\0'문자를 만날 때까지 반복
	{
		while( ! (UCSR1A & (1 << UDRE1)));	// 송신 가능 대기
		UDR1 = str[i];				// 1바이트 단위로 송신
	}
}

ISR(INT0_vect)
{
	if(flag == 0 & num == 0)
	{
		flag = 1;
	}
	if (PINB == 0x01)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}
ISR(INT1_vect)
{
	if (PINB == 0x02)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}
ISR(INT2_vect)
{
	if (PINB == 0x04)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}
ISR(INT3_vect)
{
	if (PINB == 0x08)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}
ISR(INT4_vect)
{
	if (PINB == 0x10)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}
ISR(INT5_vect)
{
	if (PINB == 0x20)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}
ISR(INT6_vect)
{
	if (PINB == 0x40)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}
ISR(INT7_vect)
{
	if (PINB == 0x80)
	{
		PORTB = 0x00;
		cnt += 1;
		if (level == 1)	_delay_ms(500);
		if (level == 2)	_delay_ms(200);
	}
}

int main(void)
{
	DDRB = 0xFF;
	cli();
	EICRA = 0xFF;
	EICRB = 0xFF;
	EIMSK = 0xFF;
	sei();
	
	int i;
	char buff[10]="";
	char buff2[10]="";
	char buff3[10]="";

	
	UART1_init();
	while (1)
	{
		if(flag == 1)
		{
			if (num == 30)	break;
			num += 1;
			sprintf(buff,"num: %d\r",num);
			UART1_print_string(buff);
			ran = rand()%8;
			sprintf(buff3,"ran: %d\r",ran);
			UART1_print_string(buff3);
			if (level == 1)
			{
				PORTB = 0x01<<ran;
				_delay_ms(500);
				PORTB = 0x00;
				_delay_ms(500);
			}
			if (level == 2)
			{
				PORTB = 0x01<<ran;
				_delay_ms(300);
				PORTB = 0x00;
				_delay_ms(300);
			}
			
			sprintf(buff2,"score: %d\r\n",cnt);
			UART1_print_string(buff2);
			if(cnt == 5	)	level = 2;
		}
	}
	UART1_print_string("----------\r\n");
}
