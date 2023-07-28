#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

void UART1_init(void);
void UART1_transmit(char data);
unsigned char UART1_receive(void);

void UART1_init(void)
{
	UBRR1H = 0x00; // 9600 ������ ����
	UBRR1L = 207;

	UCSR1A |= (  1<< U2X1); // 2��� ���
	// �񵿱�, 8��Ʈ ������, �и�Ƽ ����, 1��Ʈ ���� ��Ʈ ���
	UCSR1C |= 0x06;

	UCSR1B |= (1 << RXEN1); // �ۼ��� ����
	UCSR1B |= (1 << TXEN1);
}

void UART1_transmit(char data)
{
	while( !(UCSR1A & (1 << UDRE1)) ); // �۽� ���� ���
	UDR1 = data; // ������ ����
}

unsigned char UART1_receive(void)
{
	while( !(UCSR1A & (1<<RXC1)) ); // ������ ���� ���
	return UDR1;
}

int main(void)
{
	UART1_init(); // UART1 �ʱ�ȭ

	while(1)
	{
		UART1_transmit(UART1_receive());
	}

	return 0;
}
