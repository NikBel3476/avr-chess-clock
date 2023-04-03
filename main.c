#define F_CPU 16000000UL // Директива установки частоты процессора
#include <avr/io.h>
#include <avr/interrupt.h>

#define DIGIT_SEGMENT PORTF

volatile uint8_t counter = 0; // Счетчик секунд
volatile uint16_t timer1 = 60;
volatile uint16_t timer2 = 60;

typedef struct {
	uint8_t first : 4;
	uint8_t second : 4;
	uint8_t third : 4;
	uint8_t fourth : 4;
} timerSegment;

// void increment_single_segment(uint8_t *segment_value) {
// 	*segment_value = *segment_value >= 9 ? 0 : *segment_value + 1;
// }

// void decrement_single_segment(uint8_t *segment_value) {
// 	*segment_value = *segment_value <= 0 ? 9 : *segment_value - 1;
// }

// void increment_segment(timerSegment *segment) {

// }

// void decrement_segment(timerSegment *segment) {

// }

void update_segment(uint16_t timer, timerSegment* segment) {
	uint16_t minutes = timer / 60;
	uint8_t seconds = timer % 60;
	segment->first = seconds % 10;
	segment->second = seconds / 10;
	segment->third = minutes % 10;
	segment->fourth = minutes / 10;
}

int main() {
	// SREG &= ~(1 << 7);
	// Настройка вывода управления светодиодом
	DDRE |= 0b11111111; // All E register is output(disable input)
	DDRF |= 0b11111111; // All F register is output
	PORTE &= ~(1 << PB0); // Выключен на старте МК

	timerSegment segment1 = { 0, 0, 0, 0 };
	timerSegment segment2 = { 0, 0, 0, 0 };

	// Настройка таймера
	// TCCR1A = 0;
	// TCCR1B = (1 << CS12) | (0 << CS11) | (1 << CS10); // Делитель 1024
	// OCR1A = 0x3D09;																		// 15625 — 1 секунда
	// TCNT1 = 0;																				// Обнуление счетчика
	// TIMSK1 = (1 << OCIE1A);														// Активация прерывания для данного таймера

	// SREG |= (1 << 7);

	while (1)
	{
		// if (counter == 5)
		// {
		// 	PORTE |= (1 << PE0);
		// }
		// else if (counter == 10)
		// {
		// 	PORTE &= ~(1 << PE0);
		// 	counter = 0;
		// }
		update_segment(timer1, &segment1);
		update_segment(timer2, &segment2);

		// segment 1 digit 1
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment1.first;
		DDRE = 0b11111110;

		// segment 1 digit 2
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment1.second;
		DDRE = 0b11111101;

		// segment 1 digit 3
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment1.third;
		DDRE = 0b11111011;

		// segment 1 digit 4
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment1.first;
		DDRE = 0b11110111;

		// segment 2 digit 1
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment2.first;
		DDRE = 0b11101111;

		// segment 2 digit 2
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment2.second;
		DDRE = 0b11011111;

		// segment 2 digit 3
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment1.third;
		DDRE = 0b10111111;

		// segment 2 digit 4
		DDRE |= 0b11111111;
		DIGIT_SEGMENT &= segment1.fourth;
		DDRE = 0b01111111;
	}
}

ISR(TIMER1_COMPA_vect)
{
	counter++;
	TCNT1 = 0;
}
