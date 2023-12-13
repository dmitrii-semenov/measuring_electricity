//    Pin map for modules
//-------Current Sensor------
//   ACS712         OUT => A0
//----------Button-----------
//                  SW => D2
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
#include <adc.h>            // Library with custom ADC functions
#include <code_functions.h> // Library with some code function, for space optimization

/* Pins definitions --------------------------------------------------*/
#define SW PD2                                                      // Button 

/* Parameters --------------------------------------------------------*/
#define AVERAGE_FACTOR 1                                            // Number of averages, 1 - no average(10 recommended)
#define REF_V 5                                                     // Reference voltage for calculations
#define REF_R 0                                                     // Reference resistance for calculations

/* Global variables --------------------------------------------------*/
volatile uint8_t mode = 0;                                          // Mode of measurement
volatile uint8_t SW_ena = 0;                                        // Button sensor
volatile float ADC_avg_internal = 0;
volatile float ADC_avg = 0;                                         // Averaged ADC value 
static float Sensor_Off = -1;                                        // Offset (ADC and sensor, used for calibration)

float current_meas;                                                 // Calculated value of current
float voltage_meas;                                                 // Calculated value of voltage
float resistance_meas;                                              // Calculated value of resistance
float capacitance_meas;                                             // Calculated value of capacitance

void Clear_values(void)
{
    oled_gotoxy(14, 1);
    oled_puts("       ");
    oled_gotoxy(14, 2);
    oled_puts("       ");
    oled_gotoxy(14, 3);
    oled_puts("       ");
    oled_gotoxy(14, 4);
    oled_puts("       ");
}

int main(void)
{
    ADMUX |= (1<<REFS0);
    ADCSRA |= (1<<ADEN);
    ADCSRA |= (1<<ADIE);
    ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
    ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));

    twi_init();                                                     // TWI

    GPIO_mode_input_pullup(&DDRD, SW);                              // Pullup for button monitor

    TIM1_OVF_33MS                                                   // Timer1 enable for 33ms period
    TIM1_OVF_ENABLE

    TIM0_OVF_16MS                                                   // Timer0 enable for 16ms period
    TIM0_OVF_ENABLE                                                  

    oled_init(OLED_DISP_ON);                                        
    oled_clrscr();
    oled_charMode(NORMALSIZE);
    oled_puts("Measuring:");
    oled_gotoxy(0, 1);
    oled_puts("Current");
    oled_gotoxy(0, 2);
    oled_puts("Voltage");
    oled_gotoxy(0, 3);
    oled_puts("Resistance");
    oled_gotoxy(0, 4);
    oled_puts("Capacitance");
    oled_gotoxy(0, 5);
    oled_puts("-------------------------");
    oled_gotoxy(0, 6);
    oled_puts("ADC value:");

    EICRA |= ((1 << ISC01) | (1 << ISC00));                         // Pin change interrupt definition
    EIMSK |= (1 << INT0);

    sei();

    while (1) {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }
    // Will never reach this
    return 0;
}

ISR(INT0_vect)                                                      // Button monitor
{
    SW_ena++;
}

ISR(TIMER0_OVF_vect)                                                // ADC value read every 100ms
{
    static uint8_t no_of_overflows = 0;
    static uint8_t no_average = 0;
    no_of_overflows++;
    if (no_of_overflows >= 3)
    {
        // Do this every 3 x 16 ms = 50 ms
        no_of_overflows = 0;
        no_average++;
        ADCSRA |= (1<<ADSC);
        ADC_avg_internal = ADC_avg_internal + ADC/AVERAGE_FACTOR;
        if (no_average >= AVERAGE_FACTOR)
        {
            no_average = 0;
            ADC_avg = ADC_avg_internal;
            ADC_avg_internal = 0;
        }

    }
    // Else do nothing and exit the ISR     
}

ISR(TIMER1_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    no_of_overflows++;
    if (no_of_overflows >= 10)
    {
        if (SW_ena > 0)                                             // If button was pressed, change the state
        {
            cli();                                                  // Disable interrupt
            // Do this every 3 x 33 ms = 100 ms
            no_of_overflows = 0;
            SW_ena = 0;
            mode++;
            if (mode == 4) mode = 0;
            sei();                                                  // Enable interrupt
        }
    }
    // Else do nothing and exit the ISR     
}

ISR(ADC_vect)
{
    char string[2];                                                 // String for converted numbers by itoa()
    float value;
    float value_avg;

    value = ADC*(5.00/1023.00);                                     // Assume that ADC has a 5V reference
    
    oled_gotoxy(13, 6);
    oled_puts("    ");
    oled_gotoxy(13, 6);
    itoa(ADC_avg,string,10);
    oled_puts(string);

    value_avg = ADC_avg*(5.00/1023.00); 

    current_meas = ((value_avg*1000.00)-2500.00)/185.00 + Sensor_Off;

    switch (mode)
    {
        case 0:
            dtostrf(current_meas,5,2,string);
            oled_gotoxy(13, 4);
            oled_puts("        ");
            oled_gotoxy(13, 1);
            oled_puts(string);
        break;
            
        case 1:
            voltage_meas = value_avg*1000;
            dtostrf(voltage_meas,5,2,string);
            oled_gotoxy(13, 1);
            oled_puts("        ");
            oled_gotoxy(13, 2);
            oled_puts(string);
        break;

        case 2:
            resistance_meas = REF_V / current_meas - REF_R;
            dtostrf(resistance_meas,5,2,string);
            oled_gotoxy(13, 2);
            oled_puts("        ");
            oled_gotoxy(13, 3);
            oled_puts(string);
        break;

        case 3:
            capacitance_meas = value_avg;
            //dtostrf(capacitance_meas,5,2,string);
            oled_gotoxy(13, 3);
            oled_puts("        ");
            oled_gotoxy(13, 4);
            oled_puts("TBD");
        break;
    }
    oled_display();
}
