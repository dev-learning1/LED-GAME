/*
 * test.c
 *
 * Created: 2021-08-19 오전 12:29:41
 * Author : heejeong
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#define F_CPU 16000000L
#include <util/delay.h>

#define sbi(x, y) (x |= (1 << y))  // x의 y 비트를 설정(1)
#define cbi(x, y) (x &= ~(1 << y)) // x의 y 비트를 클리어(0)

// CON 포트는 포트 C와 연결됨을 정의
#define LCD_CON      PORTC
// DATA 포트는 포트 A와 연결됨을 정의
#define LCD_DATA     PORTA
// DATA 포트의 출력 방향 설정 매크로를 정의
#define LCD_DATA_DIR DDRA
// DATA 포트의 입력 방향 설정 매크로를 정의
#define LCD_DATA_IN  PINA
// RS 신호의 비트 번호 정의
#define LCD_RS   0
// RW 신호의 비트 번호 정의
#define LCD_RW   1
// E 신호의 비트 번호 정의
#define LCD_E    2

#define C 262 // 도
#define D 294 // 레
#define E 330 // 미
#define F 349 // 파
#define G 392 // 솔
#define A 440 // 라
#define HC 523 // 높은 도
#define PAUSE 0
#define PRESCALER 1

unsigned char ran;
volatile int cnt=0;
volatile int num=0;
volatile int flag=0;
volatile int level = 1;

const int melody_START[] = {C,E,G, PAUSE};
const int melody_END[] = {G,E,C, PAUSE};
const int melody_1[] = {G, G, A, A, G, G, E, PAUSE};
const int melody_2[] = {G, G, E, E, D, PAUSE};
const int melody_3[] = {G, G, A, A, G, G, E, PAUSE};
const int melody_4[] = {G, E, D, E, C, PAUSE};
	
//스피커 관련 함수
void init_music()
{
	DDRE |= (1 <<PE3);
	
	TCCR3A |= _BV(COM3A0);
	
	TCCR3B |= _BV(WGM32) | _BV(CS30);
}

void play_music(const int* pMusicNotes)
{
	int note;
	
	while (*pMusicNotes)
	{
		note = *pMusicNotes;
		pMusicNotes++;
		
		if (note == PAUSE)
		{
			OCR3A = 0;
		}
		else{
			int ocr_value = F_CPU / 2 / PRESCALER / note;
			
			OCR3A = ocr_value;
		}
		_delay_ms(500);
	}
	OCR3A = 0;
}

// 텍스트 LCD로 부터 상태(명령)를 읽는 함수
unsigned char LCD_rCommand(void){
	unsigned char temp=1;
	
	LCD_DATA_DIR = 0X00;
	
	cbi(LCD_CON, LCD_RS); // 0번 비트 클리어, RS = 0, 명령
	sbi(LCD_CON, LCD_RW); // 1번 비트 설정, RW = 1, 읽기
	sbi(LCD_CON, LCD_E);  // 2번 비트 설정, E = 1
	_delay_us(1);
	
	temp = LCD_DATA_IN;      // 명령 읽기
	_delay_us(1);
	
	cbi(LCD_CON, LCD_E);  // 명령 읽기 동작 끝
	
	LCD_DATA_DIR = 0XFF;
	_delay_us(1);
	
	return temp;
}

// 텍스트 LCD의 비지 플래그 상태를 확인하는 함수
char LCD_BusyCheck(unsigned char temp){
	if(temp & 0x80)          return 1;
	else            return 0;
}

// 텍스트 LCD에 명령을 출력하는 함수 - 단, 비지플래그 체크하지 않음
void LCD_wCommand(char cmd){
	cbi(LCD_CON, LCD_RS); // 0번 비트 클리어, RS = 0, 명령
	cbi(LCD_CON, LCD_RW); // 1번 비트 클리어, RW = 0, 쓰기
	sbi(LCD_CON, LCD_E);  // 2번 비트 설정, E = 1
	
	LCD_DATA = cmd;          // 명령 출력
	_delay_us(1);
	cbi(LCD_CON, LCD_E);  // 명령 쓰기 동작 끝
	
	_delay_us(1);
}

// 텍스트 LCD에 명령을 출력하는 함수 - 단, 비지플래그 체크함
void LCD_wBCommand(char cmd){
	while(LCD_BusyCheck(LCD_rCommand()))
	_delay_us(1);
	cbi(LCD_CON, LCD_RS); // 0번 비트 클리어, RS = 0, 명령
	cbi(LCD_CON, LCD_RW); // 1번 비트 클리어, RW = 0, 쓰기
	sbi(LCD_CON, LCD_E);  // 2번 비트 설정, E = 1
	
	LCD_DATA = cmd;          // 명령 출력
	_delay_us(1);
	cbi(LCD_CON, LCD_E);  // 명령 쓰기 동작 끝
	
	_delay_us(1);
}

// 텍스트 LCD를 초기화하는 함수
void LCD_Init(void){
	_delay_ms(100);
	// 비지 플래그를 체크하지 않는 Function Set
	LCD_wCommand(0x38);
	_delay_ms(10);
	// 비지 플래그를 체크하지 않는 Function Set
	LCD_wCommand(0x38);
	_delay_us(200);
	// 비지 플래그를 체크하지 않는 Function Set
	LCD_wCommand(0x38);
	_delay_us(200);
	
	// 비지 플래그를 체크하는 Function Set
	LCD_wBCommand(0x38);
	// 비지 플래그를 체크하는 Display On/Off Control
	LCD_wBCommand(0x0c);
	// 비지 플래그를 체크하는 Clear Display
	LCD_wBCommand(0x01);
}

// 텍스트 LCD에 1바이트 데이터를 출력하는 함수
void LCD_wData(char dat){
	while(LCD_BusyCheck(LCD_rCommand()))
	_delay_us(1);
	
	sbi(LCD_CON, LCD_RS); // 0번 비트 설정, RS = 1, 데이터
	cbi(LCD_CON, LCD_RW); // 1번 비트 클리어, RW = 0, 쓰기
	sbi(LCD_CON, LCD_E); // 2번 비트 설정, E = 1
	
	LCD_DATA = dat;       // 데이터 출력
	_delay_us(1);
	cbi(LCD_CON, LCD_E);  // 데이터 쓰기 동작 끝
	
	_delay_us(1);
}

// 텍스트 LCD에 문자열을 출력하는 함수
void LCD_wString(char *str){
	while(*str)
	LCD_wData(*str++);
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

ISR(INT0_vect)
{
	if(flag == 0 & num == 0)
	{
		flag = 1;
		play_music(melody_START);
		_delay_ms(1000);
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
	
	// 포트 A의 방향 설정, 0 : 입력, 1 : 출력
	DDRA = 0B11111111;
	
	// 포트 C의 방향 설정, 0 : 입력, 1 : 출력
	DDRC = 0B11111111;
	
	char buff[10]="";
	char buff1[10]="";
	
	LCD_Init();         // 텍스트 LCD 초기화 - 함수 호출
	init_music();
	
	while (1)
	{
		if(flag == 1)
		{
			LCD_wBCommand(0x80 | 0x00);  // DDRAM Address = 0 설정
			sprintf(buff,"Level: %d",level);
			LCD_wString(buff); // WESNET 문자열 출력
			num += 1;
			ran = rand()%8;
			
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
			if ((cnt == 5) & (level == 1))
			{
				play_music(melody_3);
				_delay_ms(600);
				play_music(melody_4);
				_delay_ms(600);
				
				level = 2;
			}
			if (num == 20)
			{
				//play_music(melody_1);
				//_delay_ms(600);
				//play_music(melody_2);
				//_delay_ms(1500);
				
				play_music(melody_END);
				_delay_ms(1000);
				break;
			}
		}
	}
	LCD_wBCommand(0x80 | 0x40);  // DDRAM Address = 0x40 설정
	sprintf(buff1,"score: %d",cnt);
	LCD_wString(buff1); // WESNET 문자열 출력

}