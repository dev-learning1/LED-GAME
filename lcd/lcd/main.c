/*
 * lcd.c
 *
 * Created: 2021-07-11 오후 8:14:51
 * Author : heejeong
 */ 

// ATmega128의 레지스터 등이 정의되어 있음
#include <avr/io.h>
#define F_CPU 16000000L


// _delay_ms() 함수 등이 정의되어 있음
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

// C 언어의 주 실행 함수
int main(void){
	// 포트 A의 방향 설정, 0 : 입력, 1 : 출력
	DDRA = 0B11111111;
	
	// 포트 C의 방향 설정, 0 : 입력, 1 : 출력
	DDRC = 0B11111111;
	
	unsigned int cnt;
	char buff[10]="";
	
	cnt =90;
	
	LCD_Init();         // 텍스트 LCD 초기화 - 함수 호출

	
	LCD_wBCommand(0x80 | 0x00);  // DDRAM Address = 0 설정
	LCD_wString("TEXT LCD");     // 텍스트 LCD 문자열 출력
	
	
	LCD_wBCommand(0x80 | 0x40);  // DDRAM Address = 0x40 설정
	sprintf(buff,"score: %d",cnt);
	LCD_wString(buff); // WESNET 문자열 출력
	
	// 함수의 형태와 같이 정수형(int)의 값을 반환함
	//return 1;
}
