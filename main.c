#define F_CPU 16000000UL // Директива установки частоты процессора
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#define DIGIT_SEGMENT PORTF
#define LEFT_PLAYER_BUTTON PF4
#define RIGHT_PLAYER_BUTTON PF5
#define START_STOP_BUTTON PF6
#define STARTUP_TIME_IN_SEC 60

volatile uint8_t counter = 0; // Счетчик секунд
volatile uint16_t timer1 = STARTUP_TIME_IN_SEC;
volatile uint16_t timer2 = STARTUP_TIME_IN_SEC;
volatile uint16_t timer1_remaining = 0;
volatile uint16_t timer2_remaining = 0;
volatile uint8_t is_game_running = 0;
volatile uint8_t is_left_player_time = 1;

typedef struct {
	uint8_t first : 4;
	uint8_t second : 4;
	uint8_t third : 4;
	uint8_t fourth : 4;
} timerSegment;

void update_segment(uint16_t timer, timerSegment* segment) {
	uint16_t minutes = timer / 60;
	uint8_t seconds = timer % 60;
	segment->first = seconds % 10;
	segment->second = seconds / 10;
	segment->third = minutes % 10;
	segment->fourth = minutes / 10;
}

int main() {
	DDRE |= 0b11111111;
	DDRF |= 0b10001111;

	timerSegment segment1 = { 0, 0, 0, 0 };
	timerSegment segment2 = { 0, 0, 0, 0 };

	PORTF &= 0;
	PORTE |= 0b11111111;

	TCCR1A = 0;
	TCCR1B = (1 << CS12) | (0 << CS11) | (1 << CS10);
	OCR1A = 0x3D09;
	TCNT1 = 0;
	TIMSK1 = (1 << OCIE1A);

	SREG |= (1 << 7);

	uint8_t left_player_btn_ticks_counter = 0;
	uint8_t right_player_btn_ticks_counter = 0;
	uint8_t start_stop_btn_ticks_counter = 0;

	while (1)
	{
		// left player button handler
		if ((PINF & (1 << LEFT_PLAYER_BUTTON)) == 0) {
			_delay_us(10);
			if (left_player_btn_ticks_counter == 0) {
				is_left_player_time = 1;
				timer2_remaining = TCNT1;
				TCNT1 = timer1_remaining;
			}
			if (left_player_btn_ticks_counter < 10) {
				left_player_btn_ticks_counter++;
			}
		} else {
			left_player_btn_ticks_counter = 0;
		}

		// right player button handler
		if ((PINF & (1 << RIGHT_PLAYER_BUTTON)) == 0) {
			_delay_us(10);
			if (right_player_btn_ticks_counter == 0) {
				is_left_player_time = 0;
				timer1_remaining = TCNT1;
				TCNT1 = timer2_remaining;
			}
			if (right_player_btn_ticks_counter < 10) {
				right_player_btn_ticks_counter++;
			}
		} else {
			right_player_btn_ticks_counter = 0;
		}

		// start stop button handler
		if ((PINF & (1 << START_STOP_BUTTON)) == 0) {
			_delay_ms(5);
			if (start_stop_btn_ticks_counter == 0) {
				if (timer1 == 0 || timer2 == 0) {
					timer1 = STARTUP_TIME_IN_SEC;
					timer2 = STARTUP_TIME_IN_SEC;
					is_game_running = 0;
				} else {
					is_game_running = !is_game_running;
				}
			}
			if (start_stop_btn_ticks_counter < 10) {
				start_stop_btn_ticks_counter++;
			}
		} else {
			start_stop_btn_ticks_counter = 0;
		}

		// segments update
		update_segment(timer1, &segment1);
		update_segment(timer2, &segment2);

		// segment 1 digit 1
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment1.fourth;
		PORTE &= 0b11111110;
		_delay_us(100);

		// segment 1 digit 2
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment1.third;
		PORTE &= 0b11111101;
		_delay_us(100);

		// segment 1 digit 3
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment1.second;
		PORTE &= 0b11111011;
		_delay_us(100);

		// segment 1 digit 4
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment1.first;
		PORTE &= 0b11110111;
		_delay_us(100);

		// segment 2 digit 1
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment2.fourth;
		PORTE &= 0b11101111;
		_delay_us(100);

		// segment 2 digit 2
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment2.third;
		PORTE &= 0b11011111;
		_delay_us(100);

		// segment 2 digit 3
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment2.second;
		PORTE &= 0b10111111;
		_delay_us(100);

		// segment 2 digit 4
		PORTE |= 0b11111111;
		DIGIT_SEGMENT &= 0b00000000;
		DIGIT_SEGMENT |= segment2.first;
		PORTE &= 0b01111111;
		_delay_us(100);
		PORTE |= 0b11111111;
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (is_game_running == 1 && timer1 > 0 && timer2 > 0) {
		if (is_left_player_time) {
			timer1--;
		} else {
			timer2--;
		}
	}
	TCNT1 = 0;
}
