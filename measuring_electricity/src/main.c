//    Pin map for modules
//-------Current Sensor------
//   ACS712         OUT => A0
//----------Button-----------
//                  SW => D2
//----------------------------
//      Measurement modes
// mode 0: current
// mode 1: voltage
// mode 2: resistance
// mode 3: capacitance

/* Defines -----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000     // CPU frequency in Hz required for UART_BAUD_SELECT
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <stdlib.h>         // C library. Needed for number conversions
#include <oled.h>           // Oled library for the display
#include <gpio.h>           // GPIO library for reading values from the inputs
#include "adc.h"            // Library with custom ADC functions

/* Pins definitions --------------------------------------------------*/
#define SW PD2                                                              // Button 

/* Parameters --------------------------------------------------------*/
#define AVERAGE_FACTOR 10                                                   // Number of averages, 1 - no average(10 recommended)
#define REF_V 5                                                             // Reference voltage for calculations
#define REF_R 0                                                             // Reference resistance for calculations

/* Global variables --------------------------------------------------*/
volatile uint8_t mode = 0;                                                  // Mode of measurement
volatile uint8_t SW_ena = 0;                                                // Button sensor
volatile float ADC_avg_internal = 0;
volatile float ADC_avg = 0;                                                 // Averaged ADC value 
volatile float Cap_charge = 0;                                              // Charge of the capacitance
static float Sensor_Off = -0.5;                                                // Offset (ADC and sensor, used for calibration)

float current_meas;                                                         // Calculated value of current
float voltage_meas;                                                         // Calculated value of voltage
float resistance_meas;                                                      // Calculated value of resistance
float capacitance_meas;                                                     // Calculated value of capacitance

void Clear_values(void)                                                     // Function for clearing display values
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
    ADC_5V_ref_ena                                                          // ADC configuration
    ADC_ena
    ADC_A0_read
    ADC_prescaler_128

    GPIO_mode_input_pullup(&DDRD, SW);                                      // Pullup for button monitor

    twi_init();

    TIM1_OVF_33MS                                                           // Timer1 enable for 33ms period
    TIM1_OVF_ENABLE

    TIM0_OVF_16MS                                                           // Timer0 enable for 16ms period
    TIM0_OVF_ENABLE                                                  

    oled_init(OLED_DISP_ON);                                                // Initial UI
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

    if (AVERAGE_FACTOR > 1)                                                 // Average enable indicator
    {
        oled_gotoxy(0, 7);
        oled_puts("AVERAGE ON");
    }
    else
    {
        oled_gotoxy(0, 7);
        oled_puts("AVERAGE OFF");
    }

    EICRA |= ((1 << ISC01) | (1 << ISC00));                                 // Pin change interrupt definition (PD2)
    EIMSK |= (1 << INT0);

    sei();                                                                  // Enable interrupt

    while (1) {                                                             // Empty loop
    }
    return 0;                                                               // Will never reach this
}

ISR(INT0_vect)                                                              // Button monitor
{
    SW_ena++;
}

ISR(TIMER0_OVF_vect)                                                        // ADC value read every 50ms
{
    static uint8_t no_of_overflows = 0;
    static uint8_t no_average = 0;
    no_of_overflows++;
    if (no_of_overflows >= 3)                                               // Triggers every 3 x 16 ms = 50 ms
    {
        no_of_overflows = 0;
        no_average++;
        ADC_read                                                            // Read ADC value
        ADC_avg_internal = ADC_avg_internal + ADC/AVERAGE_FACTOR;           // Calculate average ADC value
        if (no_average >= AVERAGE_FACTOR)                                   // Sent calculated value to "ADC_avg"
        {
            no_average = 0;
            ADC_avg = ADC_avg_internal;
            ADC_avg_internal = 0;
        }

    }    
}

ISR(TIMER1_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    no_of_overflows++;
    if (no_of_overflows >= 10)                                              // Triggers every 10 x 33 ms = 330 ms 
    {
        if (SW_ena > 0)                                                     // If button was pressed, change the state
        {
            cli();                                                          // Disable interrupt
            no_of_overflows = 0;
            SW_ena = 0;
            mode++;                                                         // Change state
            if (mode == 4) mode = 0;
            sei();                                                          // Enable interrupt
        }
    }   
}

ISR(ADC_vect)
{
    char string[2];                                                         // String for converted numbers by itoa()
    //float value;
    float value_avg;
    float curr_int;

    //value = ADC*(5.00/1023.00);                                           // Calculate voltage value
    
    oled_gotoxy(13, 6);                                                     // Display averaged ADC value
    oled_puts("    ");
    oled_gotoxy(13, 6);
    itoa(ADC_avg,string,10);
    oled_puts(string);

    value_avg = ADC_avg*(5.00/1024.00);                                     // Calculate voltage averaged value

    current_meas = ((value_avg*1000.00)-2500.00)/185.00 + Sensor_Off;       // Calculate current value

    switch (mode)
    {
        case 0:
            Cap_charge = 0; 
            if (current_meas < 1)                                           // If less than 1A, convert to mA
            {
                curr_int = current_meas*1000;
                dtostrf(curr_int,5,2,string);                               // Convert float to string
                oled_gotoxy(13, 4);                                         // Clear previous value
                oled_puts("          ");
                oled_gotoxy(13, 1);                                         // Display current
                oled_puts(string);
                oled_puts(" mA");
            }
            else
            {
                dtostrf(current_meas,5,2,string);                           // Convert float to string
                oled_gotoxy(13, 4);                                         // Clear previous value
                oled_puts("           ");
                oled_gotoxy(13, 1);                                         // Display current
                oled_puts(string);
                oled_puts(" A");
            }
            
        break;
            
        case 1:
            Cap_charge = 0; 
            voltage_meas = value_avg*1000;
            if (voltage_meas > 999)                                         // If bigger than 1000mV, convert to V
            {
                dtostrf(voltage_meas,5,2,string);                           // Convert float to string
                oled_gotoxy(13, 1);                                         // Clear previous value
                oled_puts("            ");
                oled_gotoxy(13, 2);                                         // Display voltage
                oled_puts(string);
                oled_puts(" V");
            }
            else
            {
                voltage_meas = value_avg/1000;
                dtostrf(voltage_meas,5,2,string);                           // Convert float to string
                oled_gotoxy(13, 1);                                         // Clear previous value
                oled_puts("            ");
                oled_gotoxy(13, 2);                                         // Display voltage
                oled_puts(string);
                oled_puts(" mV");
            }
        break;

        case 2:
            Cap_charge = 0; 
            resistance_meas = REF_V / current_meas - REF_R;
            if (resistance_meas > 999)                                      // If bigger than 1000 Ohm, convert to kOhm
            {
                resistance_meas = resistance_meas/1000;
                dtostrf(resistance_meas,5,2,string);                        // Convert float to string
                oled_gotoxy(13, 2);                                         // Clear previous value
                oled_puts("            ");
                oled_gotoxy(13, 3);                                         // Display resistance
                oled_puts(string);
                oled_puts(" kO");
            }
            else
            {
                dtostrf(resistance_meas,5,2,string);                        // Convert float to string
                oled_gotoxy(13, 2);                                         // Clear previous value
                oled_puts("            ");
                oled_gotoxy(13, 3);                                         // Display resistance
                oled_puts(string);
                oled_puts(" O");
            }
            
        break;

        case 3:
            Cap_charge = Cap_charge + current_meas*0.05;                    // Calculate charge as integral of current
                                                                            // 0.05 is 50 ms (ADC period)
            capacitance_meas = 1000000.00 * Cap_charge / 5.00;              // Calculate capacitance in uF
            if (capacitance_meas > 999)
            {
                capacitance_meas = capacitance_meas/1000;                   // If bigger than 1000 uF, convert to mF
                dtostrf(capacitance_meas,5,2,string);                       // Convert float to string
                oled_gotoxy(13, 3);                                         // Clear previous value
                oled_puts("            ");
                oled_gotoxy(13, 4);                                         // Display capacitance
                oled_puts(string);
                oled_puts(" mF");
            }
            else if (capacitance_meas > 999999)
            {
                capacitance_meas = capacitance_meas/1000000;                // If bigger than 1000 mF, convert to F
                dtostrf(capacitance_meas,5,2,string);                       // Convert float to string
                oled_gotoxy(13, 3);                                         // Clear previous value
                oled_puts("            ");
                oled_gotoxy(13, 4);                                         // Display capacitance
                oled_puts(string);
                oled_puts(" F");
            }
            else
            {
                dtostrf(capacitance_meas,5,2,string);                       // Convert float to string
                oled_gotoxy(13, 3);                                         // Clear previous value
                oled_puts("            ");
                oled_gotoxy(13, 4);                                         // Display capacitance
                oled_puts(string);
                oled_puts(" uF");
            }
            
        break;
    }
    oled_display();                                                         // Display
}
