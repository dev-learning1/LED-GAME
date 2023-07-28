/*
 * availableResistor.c
 *
 * Created: 2022-03-26 오후 7:45:08
 * Author : heejeong
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

void ADC_init()
{
	ADMUX |= (1 << REFS0);	//기준전압 5V
	ADMUX |= (1 << MUX1 | 1 << MUX0);
	// PF3을 ADC입력핀으로 설정. 단일 입력 방식
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	//분주비 128
	ADCSRA |= (1 << ADEN);	//ADC 활성화
	ADCSRA |= (1 << ADFR);	//프리러닝 모드
	ADCSRA |= (1 << ADSC);	//ADC 시작
}
int read_ADC(void)
{
	while(! (ADCSRA & (1 << ADIF)));
	return ADC;
}

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
void UART1_print_data(int no)
{
	int radix = 10;
	char buffer[sizeof(int) * 8 +1];
	itoa(no, buffer, radix);
	UART1_print_string(buffer);
}
int main()
{
	int read;
	ADC_init();
	UART1_init();
	while(1)
	{
		read = read_ADC();
		UART1_print_data(read);
		//sprintf(buff,"score: %d",cnt);
		//UART1_print_string(buff);
		_delay_ms(1000);
	}
	return 0;
}