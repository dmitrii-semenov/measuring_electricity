// Pin map for connected modules
// Current Sensor
//      OUT => A3
//  Potenciometer
//       SW => D2
//       DT => D3
//      CLK => D4

/* Defines -----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000  // CPU frequency in Hz required for UART_BAUD_SELECT
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <twi.h>            // I2C/TWI library for AVR-GCC
#include <uart.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions
#include <oled.h>
#include <gpio.h>

/* Pins definitions --------------------------------------------------*/
#define SW PD2   // CLK is a pin for potenciometer output 
#define DT PD3    // DT is a pin for potenciometer output
#define CLK PD4   // CLK is a pin for potenciometer output 

uint8_t state = 0;
uint8_t state_rotary = 0;

int main(void)
{
    // Configure ADC
    ADMUX = ADMUX | (1<<REFS0); 
    ADMUX = ADMUX | (0<<REFS1); 
    // Select input channel ADC0 (voltage divider pin)
    ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
    // Enable ADC module
    ADCSRA = ADCSRA | (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRA = ADCSRA | (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA = ADCSRA | (1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0);

    twi_init(); //TWI
    uart_init(UART_BAUD_SELECT(115200, F_CPU)); //UART

    // Timer1 enable
    TIM1_OVF_33MS
    TIM1_OVF_ENABLE

    // Timer0 enable for 1 milisecond period
    TIM0_OVF_16MS
    TIM0_OVF_ENABLE

    sei(); 

    // Initial UI
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    oled_charMode(NORMALSIZE);
    oled_gotoxy(0, 0);
    oled_puts("Select mode:");
    oled_gotoxy(0, 1);
    oled_puts("1 - Current");
    oled_gotoxy(0, 2);
    oled_puts("2 - Voltage");
    oled_gotoxy(0, 3);
    oled_puts("3 - Resistance");
    oled_gotoxy(0, 4);
    oled_puts("4 - Capacitance");
    oled_gotoxy(0, 5);
    oled_puts("-------------------------");
    oled_gotoxy(0, 6);
    oled_puts("Rotate potentiometer");
    oled_gotoxy(0, 7);
    oled_puts("to choose mode");
    oled_display();
}

ISR(TIMER0_OVF_vect)
{
    uint8_t DT_read;
    uint8_t CLK_read;
    uint8_t SW_read;
    SW_read = GPIO_read(&PIND, PD2);
    DT_read = GPIO_read(&PIND, PD3);
    CLK_read = GPIO_read(&PIND, PD4);

    if (SW_read == 0)
    {
        state = 1; // Then change to rotary encoder value
    }

    char string[2];  // String for converted numbers by itoa()
    itoa(DT_read, string, 10);
    oled_gotoxy(0, 4);
    oled_puts("    ");
    oled_gotoxy(0, 4);
    oled_puts(string);

    itoa(CLK_read, string, 10);
    oled_gotoxy(0, 5);
    oled_puts("    ");
    oled_gotoxy(0, 5);
    oled_puts(string);

    itoa(SW_read, string, 10);
    oled_gotoxy(0, 6);
    oled_puts("    ");
    oled_gotoxy(0, 6);
    oled_puts(string);

    itoa(state, string, 10);
    oled_gotoxy(0, 7);
    oled_puts("    ");
    oled_gotoxy(0, 7);
    oled_puts(string);
}

ISR(TIMER1_OVF_vect)
{
    ADCSRA = ADCSRA | (1<<ADSC);
}

ISR(ADC_vect)
{
    uint16_t value;
    float mV_curr_sens;
    char string[6];  // String for converted numbers by itoa()
    value = ADC;
    mV_curr_sens = (value-512)*1000;
    itoa(mV_curr_sens, string, 10);
    oled_gotoxy(0, 0);
    oled_puts("    ");
    oled_gotoxy(0, 0);
    oled_puts(string);

    itoa(value, string, 10);
    oled_gotoxy(0, 2);
    oled_puts("    ");
    oled_gotoxy(0, 2);
    oled_puts(string);
    oled_display();
}