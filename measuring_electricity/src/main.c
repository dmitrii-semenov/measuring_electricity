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

/* Pins definitions --------------------------------------------------*/
#define DT PD3    // DT is a pin for potenciometer output
#define CLK PD4   // CLK is a pin for potenciometer output 

int main(void)
{
    twi_init(); //TWI
    uart_init(UART_BAUD_SELECT(115200, F_CPU)); //UART
    sei();  // Needed for UART

    // Timer1 enable for 1 second period
    TIM1_OVF_1SEC
    TIM1_OVF_ENABLE

    // Timer0 enable for 16 microsecond period
    TIM0_OVF_16US
    TIM0_OVF_ENABLE

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

ISR(TIMER1_OVF_vect)
{
}

ISR(TIMER0_OVF_vect)
{
}