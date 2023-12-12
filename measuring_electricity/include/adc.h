#ifndef ADC_H
# define ADC_H

/***********************************************************************
 * 
 * ADC library for AVR-GCC.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2023 Dmitrii Semenov, Lunin Roman, Karmanov Vladislav
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

/**
 * @file 
 * @defgroup Measuring electricity <adc.h>
 * @code #include <adc.h> @endcode
 *
 * @brief ADC library for AVR-GCC.
 *
 * The library contains macros for controlling the adc module.
 *
 * @note Based on Microchip Atmel ATmega328P manual and no source file
 *       is needed for the library.
 * @author Dmitrii Semenov, Lunin Roman, Karmanov Vladislav, Dept. of Radio Electronics, Brno University 
 *         of Technology, Czechia
 * @copyright (c) 2023 Dmitrii Semenov, Lunin Roman, Karmanov Vladislav, This work is licensed under 
 *                the terms of the MIT license
 * @{
 */


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>


/* Defines -----------------------------------------------------------*/
/**
 * @name  Definitions for ADC
 * @note  ADC has a limit of 1023
 */
/** @brief Enable 5V reference to the ADC */
#define ADC_5V_ref_ena      ADMUX |= (1<<REFS0);

/** @brief Enable VBG reference to the ADC */
#define ADC_VBG_ref_ena      ADMUX |= (1<<REFS1); ADMUX |= (1<<REFS0);

/** @brief Set precaler for 2 */
#define ADC_prescaler_2      ADCSRA |= (1<<ADPS0); ADCSRA &= ~((1<<ADPS2) | (1<<ADPS1)); // 001

/** @brief Set precaler for 4 */
#define ADC_prescaler_4      ADCSRA |= (1<<ADPS1); ADCSRA &= ~((1<<ADPS0) | (1<<ADPS2)); // 010

/** @brief Set precaler for 8 */
#define ADC_prescaler_8      ADCSRA |= ((1<<ADPS0) | (1<<ADPS1)); ADCSRA &= ~(1<<ADPS2); // 011

/** @brief Set precaler for 16 */
#define ADC_prescaler_16      ADCSRA |= (1<<ADPS2); ADCSRA &= ~((1<<ADPS0) | (1<<ADPS1)); // 100

/** @brief Set precaler for 32 */
#define ADC_prescaler_32      ADCSRA |= ((1<<ADPS2) | (1<<ADPS0)); ADCSRA &= ~(1<<ADPS1); // 101

/** @brief Set precaler for 64 */
#define ADC_prescaler_64      ADCSRA |= ((1<<ADPS2) | (1<<ADPS1)); ADCSRA &= ~(1<<ADPS0); // 110

/** @brief Set precaler for 128 */
#define ADC_prescaler_128      ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)); // 111

/** @brief Select input pin ADC0 */
#define ADC_A0_read      ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0)); // 0000

/** @brief Select input pin ADC1 */
#define ADC_A1_read      ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1)); ADMUX |= (1<<MUX0); // 0001

/** @brief Select input pin ADC2 */
#define ADC_A2_read      ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0)); ADMUX |= (1<<MUX1); // 0010

/** @brief Select input pin ADC3 */
#define ADC_A3_read      ADMUX &= ~((1<<MUX3) | (1<<MUX2)); ADMUX |= ((1<<MUX1) | (1<<MUX0)); // 0011

/** @brief Select input pin ADC4 */
#define ADC_A4_read      ADMUX &= ~((1<<MUX3) | (1<<MUX1) | (1<<MUX0)); ADMUX |= (1<<MUX2); // 0100

/** @brief Select input pin ADC5 */
#define ADC_A5_read      ADMUX &= ~((1<<MUX3) | (1<<MUX1)); ADMUX |= ((1<<MUX2) | (1<<MUX0)); // 0101

/** @brief Select input pin ADC6 */
#define ADC_A6_read      ADMUX &= ~((1<<MUX3) | (1<<MUX0)); ADMUX |= ((1<<MUX1) | (1<<MUX2)); // 0110

/** @brief Select input pin ADC7 */
#define ADC_A7_read      ADMUX &= ~(1<<MUX3); ADMUX |= ((1<<MUX2) | (1<<MUX1) | (1<<MUX0)); // 0111

/** @brief Select input temperature sensor */
#define ADC_temp_read      ADMUX &= ~((1<<MUX2) | (1<<MUX1) | (1<<MUX0)); ADMUX |= (1<<MUX3); // 1000

/** @brief Enable ADC */
#define ADC_ena      ADCSRA |= (1<<ADEN); ADCSRA |= (1<<ADIE);

/** @brief Read ADC */
#define ADC_read      ADCSRA |= (1<<ADSC);

#endif