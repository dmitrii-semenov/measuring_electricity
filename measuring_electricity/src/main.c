//    Pin map for modules
//-------Current Sensor-------
//   ACS712         OUT => A3
//-------Potenciometer--------
//                  SW => D2
//  Rotary encoder  DT => D3
//                  CLK => D4
//----------------------------

/* Defines -----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000     // CPU frequency in Hz required for UART_BAUD_SELECT
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <twi.h>            // I2C/TWI library for AVR-GCC
#include <stdlib.h>         // C library. Needed for number conversions
#include <oled.h>           // Oled library for the display
#include <gpio.h>           // GPIO library for reading values from the inputs

/* Pins definitions --------------------------------------------------*/
#define SW PD2              // button 

int state = 0;          // State (0 - if mode is not assigned, 1 - if assigned)
int rotary_number = 0;
int previous_state = 0;
uint8_t average_steps = 10;

float Current_Get()        // Function for immediate current calculation
{
    uint16_t valueADC;
    float I_change = 0.066;
    float V_curr_sens, Current;
    valueADC = ADC;
    V_curr_sens = (valueADC-512)*(5/1024);
    Current = 1000*V_curr_sens/I_change;
    return Current;
}

int main(void)
{
    ADMUX = ADMUX | (1<<REFS0);                                     // Configure reference voltage for ADC
    ADMUX = ADMUX | (0<<REFS1); 
    ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);       // Select input channel ADC0 (voltage divider pin)
    ADCSRA = ADCSRA | (1<<ADEN);                                    // Enable ADC module
    ADCSRA = ADCSRA | (1<<ADIE);                                    // Enable conversion complete interrupt
    ADCSRA = ADCSRA | (1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0);             // Set clock prescaler to 128

    twi_init();                                                     // TWI

    TIM1_OVF_33MS                                                   // Timer1 enable for 33ms period
    TIM1_OVF_ENABLE

    sei();                                                          // Interrupt enable

    oled_init(OLED_DISP_ON);                                        // Initial UI
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

ISR(TIMER1_OVF_vect)
{
    ADCSRA = ADCSRA | (1<<ADSC);
}

ISR(ADC_vect)
{
    char string[6];  // String for converted numbers by itoa()
    uint16_t current;
    current = Current_Get();
    //current = ADC;
    itoa(current, string, 10);
    oled_gotoxy(10, 3);
    oled_puts(string);
    oled_display();
}