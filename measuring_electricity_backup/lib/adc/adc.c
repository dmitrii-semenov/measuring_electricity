#include <adc.h>

void ADC_5V_ref_ena(void)
{
    ADMUX |= (1<<REFS0);
}

void ADC_prescaler_128(void)
{
    ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));
}

void ADC_A0_read(void)
{
    ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));

}

void ADC_A1_read(void)
{
    ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1)); ADMUX |= (1<<MUX0);
}

void ADC_A2_read(void)
{
    ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0)); ADMUX |= (1<<MUX1);
}

void ADC_A3_read(void)
{
    ADMUX &= ~((1<<MUX3) | (1<<MUX2)); ADMUX |= ((1<<MUX0) | (1<<MUX1));
}

void ADC_A4_read(void)
{
    ADMUX &= ~((1<<MUX3) | (1<<MUX1) | (1<<MUX0)); ADMUX |= (1<<MUX2);
}

void ADC_A5_read(void)
{
    ADMUX &= ~((1<<MUX3) | (1<<MUX1)); ADMUX |= ((1<<MUX2) | (1<<MUX0));
}

void ADC_ena(void)
{
    ADCSRA |= (1<<ADEN);
    ADCSRA |= (1<<ADIE);
}

void ADC_read(void)
{
    ADCSRA |= (1<<ADSC);
}