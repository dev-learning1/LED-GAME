/*
 * BUZZER.c
 *
 * Created: 2021-07-18 오후 6:17:24
 * Author : heejeong
 */ 
/*#include <avr/io.h>
#include <util/delay.h>
//#include <stdlib.h>

#define F_CPU 16000000L
//#define __DELAY_BACKWARD_COMPATIBLE__

#define C 262 // 도
#define D 294 // 레
#define E 330 // 미
#define F 349 // 파
#define G 392 // 솔
#define A 440 // 라
#define B 494 // 시

void delay_ms(int ms)
{
	while(ms-- != 0)
	_delay_ms(1);
}
int buzzer(double hz, int count)
{
	int j;
	
	for (j=0;j<count;j++)
	{
		//t=1/f
		PORTF = 0x01;
		delay_ms(((double)1000/hz)/2);
		
		PORTF=0x00;
		delay_ms(((double)1000/hz)/2);
	}
}

int main(void)
{
	int i;
	DDRF = 0x01;
	while(1)
	{
		for (i=0;i<20;i++)
		{
			buzzer(C, 50);
			buzzer(D, 50);
			buzzer(E, 50);
			buzzer(F, 50);
		}
		_delay_ms(2000);
	}
}*/


/*#define C 262 // 도
#define D 294 // 레
#define E 330 // 미
#define F 349 // 파
#define G 392 // 솔
#define A 440 // 라
#define B 494 // 시

int piezoPin = 8; // 부저의 ⊕극을 오렌지 보드 8번에 연결
int tempo = 200; // duration 옵션 값 설정
int notes[25] = { G, G, A, A, G, G, E, G, G, E, E, D, G, G, A, A, G, G, E, G, E, D, E, C };

void setup() {
	pinMode (piezoPin, OUTPUT);
}

void loop() {
	for (int i = 0; i < 12; i++) {
		tone (piezoPin, notes[ i ], tempo);
		_delay_ms (300);
	}
	_delay_ms(100); // 멜로디 중간에 짧게 멈추는 용도

	for (int i = 12; i < 25; i++) {
		tone (piezoPin, notes[ i ], tempo);
		_delay_ms(300);
	}
}*/

/*use timer counter 0 */

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define OFF 0   //isr state compare value
#define ON  1   //isr state compare value
#define DO  0
#define RE  1
#define MI  2
#define FA  3
#define SO  4
#define RA  5
#define SI  6
#define DO_H 7
#define EOS  255  //(End of song)
// C(도)  523
// D(레)  587
// E(미)  659
// F(파)  699
// G(솔)  784
// A(라)  880
// B(시)  988
// C_H(도) 1047

char cb[]={238,212,189,178,158,141,126,118}; //8bit compare_interrupt:(compare TCNT0 and OCR0(use value)).

unsigned char mz[]={SO, MI, MI, SO, MI, DO, RE, MI, RE, DO, MI, SO, DO_H, SO, DO_H, SO, DO_H, SO, MI, SO, RE, FA, MI, RE, DO, EOS};
volatile unsigned char init_state = OFF, tone;
// e.g: duty cycle : 50%   half: 0 or 1.(T/2)   atmel128 16MHZ T: 0.0625us  prescale: 64 T: 4us. 8bit timer&counter : 256ea

//void init_buzzer();
//void comparei_CTC_bit_set();

ISR(TIMER0_COMP_vect)
{
	OCR0 = cb[tone];
	if(init_state==OFF)
	{
		PORTB |= (1<<PB4);
		init_state = ON;
	}
	else
	{
		PORTB &= ~(1<<PB4);
		init_state = OFF;
	}
}

void init_buzzer()
{
	DDRA = 0xff;
	DDRB = (1<<PB4);
}


void comparei_CTC_bit_set()
{
	TCCR0 = 0;
	TCCR0 |= (1<<WGM01)|(0<<WGM00);       //waveforme generate : CTC ,TOP: OCR0, update: immeiate, Max. 00, 01, (10) 11.
	TCCR0 |= (0<<COM01)|(1<<COM00);       // Toggle OC0 on compare match. (non-PWM mode)  00, 01, 10, 11 don't care in                                                              //this code.
	TCCR0 |= (1<<CS2)|(0<<CS1)|(0<<CS0);  //  prescale:64, 000:source clock  011:32, 100:64, 101:128, 110:256, 111:1024
	TIMSK = 0;
	TIMSK |= (1<<OCIE0);                //  interrupt by Compare tcnt0 (0~0xff) and OC0 (user value)valuse.
	TCNT0 = 0;
	sei();                                  //  enable global interrupt.
	
}

int main(void)
{
	int i=0;
	init_buzzer();
	comparei_CTC_bit_set();
	
	do{
		cli();
		tone= mz[i++];          //critical section. // for문으로 변경하여 변수인자형을 unsigned char (256)값으로 한정.
		sei();
		PORTA = tone;      //mz 값 확인용.
		_delay_ms(300);
		
	}while(tone!=EOS);

}


/*
 * GccApplication18.c
 *
 * Created: 2021-07-23 오후 2:27:14
 * Author : 이유림
 */ 
/*
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

//#define D5 587
//#define E5 659
//#define S_D5 622
//#define B4 493
//#define C5 523
//#define A4 440
#define C 262 // 도
#define D 294 // 레
#define E 330 // 미
#define F 349 // 파
#define G 392 // 솔
#define A 440 // 라
#define PAUSE 0
#define PRESCALER 1

//const int melody[] = {E5, S_D5, E5, S_D5, E5, B4, D5, C5, A4, PAUSE};

const int melody[] = {G, G, A, A, G, G, E, G, G, E, E, D, G, G, A, A, G, G, E, G, E, D, E, C, PAUSE};
	
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
		if (pMusicNotes == 7 | pMusicNotes == 12 | pMusicNotes == 19)
		{
			_delay_ms(1000);
		}
		_delay_ms(500);
	}
	OCR3A = 0;
}

int main(void)
{
	init_music();
	// Replace with your application code 
	while (1)
	{
		play_music(melody);
		_delay_ms(1000);
	}
	return 0;
}*/