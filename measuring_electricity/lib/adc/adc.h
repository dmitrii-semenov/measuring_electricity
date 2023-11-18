#ifndef ADC_H
# define ADC_H

#include <avr/io.h>
#include <avr/pgmspace.h>

/** @brief  ADC conversion from pin
 *  @param  inp  input port
 */
#define ADC_CONVERSION(inp) (((xtalCpu) + 8UL * (baudRate)) / (16UL * (baudRate)) - 1UL)

#endif // ADC_H