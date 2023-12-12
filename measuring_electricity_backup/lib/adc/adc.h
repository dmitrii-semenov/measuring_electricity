#ifndef ADC_H
# define ADC_H

#include <avr/io.h>

void ADC_5V_ref_ena();

void ADC_prescaler_128();

void ADC_A0_read();

void ADC_A1_read();

void ADC_A2_read();

void ADC_A3_read();

void ADC_A4_read();

void ADC_A5_read();

void ADC_ena();

void ADC_read();

#endif