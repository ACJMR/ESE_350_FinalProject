#include <Arduino.h>
#include "Mouse.h"
#include <avr/io.h>
#include "uart.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

void blink() {
    DDRC |= (1 << PC7);
    PORTC |= (1 << PC7);
    _delay_ms(1000);
    PORTC &= ~(1 << PC7);
    _delay_ms(1000);
}

void mouse_move(int offset, int dim, Mouse_* Mouseptr){
    if (dim) { //Move y
        if (offset < 0) {
            for (int i = 0; i < -offset; i++) { //150 = 130
                mouseC_move(m, 0, -3);
            }
        } else {
            for (int i = 0; i < offset; i++) { //150 = 130
                mouseC_move(m, 0, 3);
            }
        }
    } else {
        if (offset < 0) {
            for (int i = 0; i < -offset; i++) { //150 = 130
                mouseC_move(m, -3, 0);
            }
        } else {
            for (int i = 0; i < offset; i++) { //150 = 130
                mouseC_move(m, 3, 0);
            }
        }
    }
}

int process_data(unsigned char a) {
    if (a == 0) {
        return 0;
    }
    if (a <= 127) {
        return a - 128;
    } else {
        return a - 127;
    }
}

//113 pixels = 127 library
//115 pixels = 110 library
//105 pixels = 100 library
//94 pixels = 90 library
//91 pixels = 80 library
//84 pixels = 70 library
//77 pixels = 60 library
//68 pixels = 50 library
//51 pixels = 40 library
//38 pixels = 30 library
//24 pixels = 25 library
//15 pixles = 20 library
//6 pixels = 10 library
//5 pixels = 8 library
//1 pixels = 3 library

//int difs_x[] = {0, 5, -5, 3, 2, -16, -10, -15, 18, 55, 25, -16, 19, 34, 4, -46, -30, -17};
//int difs_y[] = {0, -3, -29, -37, -41, -36, -33, -25, -24, 7, -7, -9, -2, 3, -3, -5, -5, -7};
//int time_per_bullet_ms = 500;


int a_to_offset(int a) {
    return 7*a;
}

void my_init() {
    //-------------------- Timer 1 --------------------
    //Enable output pin
    DDRB |= (1 << DDB6);
    PORTB &= ~(1 << PORTB6);

    //Set timer3 to fast PWM mode
    TCCR1A |= (1 << WGM10);
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << WGM13);

    //Toggle PB6 pin on compare-match
    TCCR1A &= ~(1 << COM1B0);
    TCCR1A |= ~(1 << COM1B1);

    //Prescale by 1024
    TCCR1B |= (1 << CS12);
    TCCR1B &= ~(1 << CS11);
    TCCR1B |= (1 << CS10);

    //Manually clear the capture flag
    TIFR1 |= (1 << TOV1);
    OCR1A = 312;
    OCR1B = 24;

    //-------------------- Timer 3 --------------------
    //Enable output pin and set initial value
    DDRC |= (1 << DDC6);
    PORTC &= ~(1 << PORTC6);

    //Set timer3 to CTC mode
    TCCR3A &= ~(1 << WGM30);
    TCCR3A &= ~(1 << WGM31);
    TCCR3B |= (1 << WGM32);
    TCCR3B &= ~(1 << WGM33);

    //Toggle PC6 pin on compare-match
    TCCR3A &= ~(1 << COM3A0);
    TCCR3A &= ~(1 << COM3A1);

    //Prescale by 256
    TCCR3B |= (1 << CS32);
    TCCR3B &= ~(1 << CS31);
    TCCR3B &= ~(1 << CS30);

    //Manually clear the capture flag
    TIFR3 |= (1 << TOV3);
    OCR3A = 70;

    //-------------------- LEDs --------------------

    DDRD |= (1 << DDD0);
    DDRD |= (1 << DDD1);
    PORTD &= ~(1 << PORTD0);
    PORTD &= ~(1 << PORTD1);
}

int my_main() {
    MouseC_t *m = mouseC_create();
    blink();

    my_init();
    UART_init(BAUD_PRESCALER);

    delay(5000);
    TCCR3A |= (1 << COM3A0);
    PORTD |= (1 << PORTD0);
    PORTD |= (1 << PORTD1);
    delay(1000);

    while(1) {
        TCCR3A &= ~(1 << COM3A0);
        PORTD |= (1 << PORTD0);
        PORTD &= ~(1 << PORTD1);
        unsigned char data = USART_Receive();
        int offset_x = a_to_offset(process_data(data));

        data = USART_Receive();
        int offset_y = a_to_offset(process_data(data));
        mouse_move(offset_x, 0, m);
        mouse_move(offset_y, 1, m);

        PORTD &= ~(1 << PORTD0);
        PORTD |= (1 << PORTD1);
        TCCR3A |= (1 << COM3A0);
        m->click();
        _delay_ms(1000);
    }
}