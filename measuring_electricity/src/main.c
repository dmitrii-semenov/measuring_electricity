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
#include <uart.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions
#include <oled.h>           // Oled library for the display
#include <gpio.h>           // GPIO library for reading values from the inputs
#include <stdio.h>          // DEBUG PURPOSE ONLY!!!

/* Pins definitions --------------------------------------------------*/
#define SW PD2              // CLK is a pin for potenciometer output 
#define DT PD3              // DT is a pin for potenciometer output
#define CLK PD4             // CLK is a pin for potenciometer output 

int state = 0;          // State (0 - if mode is not assigned, 1 - if assigned)
int rotary_number = 0;
int previous_state = 0;
uint8_t average_steps = 10;

double Current_Get()        // Function for immediate current calculation
{
    uint16_t valueADC;
    double I_change = 0.066;
    double V_curr_sens, Current;
    valueADC = ADC;
    V_curr_sens = (valueADC-512)*(5/1024);
    Current = V_curr_sens/I_change;
    return Current;
}

void Rotary_value(uint8_t data_CLK, uint8_t data_DT)                // Function for rotary encoder control
{
    int current_state;

    if ((data_CLK == 0) && (data_DT == 0)) current_state = 0;
    else if ((data_CLK == 0) && (data_DT == 1)) current_state = 1;
    else if ((data_CLK == 1) && (data_DT == 0)) current_state = -1;
    else current_state = 10;

    int diff;
    diff = current_state - previous_state;

    if ((current_state == 1) && (diff == 1))
    {
        if (rotary_number < 3) ++rotary_number;
    }
    else if ((current_state == 1) && (diff == -1))
    {
        if (rotary_number > 0) --rotary_number;
    }
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
    uart_init(UART_BAUD_SELECT(115200, F_CPU));                     // UART

    TIM1_OVF_33MS                                                   // Timer1 enable for 33ms period
    TIM1_OVF_ENABLE

    TIM0_OVF_16MS
    TIM0_OVF_ENABLE                                                 // Timer0 enable for 1s period

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

ISR(TIMER0_OVF_vect)
{
    if (state == 0)
    {
        uint8_t DT_read;
        uint8_t CLK_read;
        uint8_t SW_read;
        SW_read = GPIO_read(&PIND, PD2);
        DT_read = GPIO_read(&PIND, PD3);
        CLK_read = GPIO_read(&PIND, PD4);

        if (SW_read == 1) state = 1;
        else state = 0;

        if (state == 0)
        {
            Rotary_value(CLK_read,DT_read);
            oled_gotoxy(18, rotary_number + 1);
            oled_puts("<");
        }
    }

    if (state == 1)
    {
        switch(rotary_number) 
        {
            case 0 :                    // Current
                oled_gotoxy(10, 5);
                oled_puts("C0");
            case 1 :                    // Voltage
                oled_gotoxy(10, 5);
                oled_puts("C1");
            case 2 :                    // Resistance
                oled_gotoxy(10, 5);
                oled_puts("C2");
            case 3 :                    // Capacitance
                oled_gotoxy(10, 5);
                oled_puts("C3");
        }
    }
}

ISR(TIMER1_OVF_vect)
{
    ADCSRA = ADCSRA | (1<<ADSC);
}

ISR(ADC_vect)
{
    char string[6];  // String for converted numbers by itoa()
    double current;
    current = Current_Get();
    itoa(current, string, 10);
    printf(string);
}