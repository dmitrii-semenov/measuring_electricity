# Project in C

Topic: Measuring Electricity

### Team members

* Dmitrii Semenov (responsible for debugging, C programming, and hardware description)
* Roman Lunin (responsible for C programming, Github design, and software description)
* Vladislav Karmanov (responsible for documentation, Github design, library `adc`)

## Theoretical description and explanation

The main idea is to design a device that can measure electrical quantities (we were inspired by making a custom multimeter). To implement our idea, we used a current sensor [ACS712](https://www.sparkfun.com/datasheets/BreakoutBoards/0712.pdf) with a 5A measurement limit. We aimed to make a decent interface so that measurements could be conveniently and intuitively controlled, so we used the OLED display and the button to choose a measurement mode. Since the usage of Arduino frameworks is strictly forbidden, the only option to "read" a value from the current sensor was to use `ADC`(Analog Digital Converter), which could convert an analog value from the analog input pin to the digital value. We deeply researched how it works, and using [ATmega328P](https://www.microchip.com/en-us/product/ATmega328p) specification, we were able to configure the internal registers in a way to read the values from the input analog pin (in our case A0), and subsequently process the obtained value. After that, we implemented blocks to control the button (button controller), and to switch modes. For the latter, we have made an FSM (Finite State Machine) that switches between 4 modes. Each mode corresponds to a measurement of one value (current, voltage, resistance and capacitance).

Moreover, we have developed a custom library `adc`, its use is not limited to our project: it contains basic functions for ADC configuration, so it can be used in many other projects, helping their creators.

## Application

Our proposed device can measure two basic parameters, voltage and current, which are essential for power management in batteries. Furthermore, we decided to increase the application area of the device and we developed advanced functions, such as resistance and capacity measurement. It can be used in even more advanced applications, such as verification of BJT and MOS transistors (a high potential on base, resp. gate can be applied and current on emitor, resp. source can be measured). One of the potential applications - is an overcurrent sensor. Once the current exceeds the threshold within 5A, this sensor may activate the safety mechanism and turn off the flow of current.

## Hardware description of the demo application

**Measuring of current:** According to the datasheet, the current sensor has a VCC/2 voltage on the output in a case with zero current. In our case, vCC equals 5 Volts, since we use Arduino internal voltage. To calculate the current, we transform the ADC value into volts, then subtract 2.5V and divide by sensitivity, which equals 185mV. Finally, we add an offset that may be present (or equals 0 if the sensor has no offset). One important thing is that we used the averaging process to cancel the noise from ADC. The average setup (number of averages) can be set by a global variable `AVERAGE_FACTOR`. If the value is "1", the average is disabled, and there is a note on the display "AVERAGE OFF". Otherwise, there is a note "AVERAGE ON". We recommend using at least 10 average steps for an accurate measurement. The code adjusts the display of voltage in `mA` or `A` according to its absolute value (>1A => display in A, otherwise in mA). The final equation for a current calculation is:

$$I = (ADC_{avg} \cdot \frac{5000}{1024} - 2500)/185$$

**Measuring of voltage:** The voltage is calculated directly from the ADC averaged value. The code adjusts the display of voltage in `mV` or `V` as well, according to its absolute value (>1V => display in V, otherwise in mV). The total equation is:

$$V = ADC_{avg} \cdot \frac{5}{1024}$$

**Measuring of resistance:** Resistance is calculated as a division of reference voltage `REF_V` (5V by default) and measured current. If additional resistance is used in the series connection with the measured one, its value should be assigned to the global variable `REF_R`. The code adjusts the display of resistance in `kO` (kiloOhms) or `O` (Ohms), according to its absolute value (>1000 Ohms => display in kO, otherwise in O). The total equation is:
 
$$R = \frac{V_{ref}}{I} - R_{ref}$$

**Measuring of capacitance:** Capacitance is measured according to its definition. For a capacitor, the capacitance equals the charge divided by voltage. Thus, if we apply a known voltage (5V `REF_V`) to the **discharged!** capacitor, the integral of its current in the time domain equals its charge. We know that during the charging process, the current through the capacitor decreases exponentially, so after the measurement is started, a user should wait an appropriate amount of time for current stabilization. The code adjusts the measured value, displaying it in `uF`, `mF` or `F`. The total equation is:

$$C = \frac{1}{V_{ref}} \cdot \sum_{t=0}^{t=n}I_n \cdot \Delta t$$

Our final proposed design looks as follows:

<img src="https://github.com/dmitrii-semenov/measuring_electricity/blob/main/pictures/schematic.JPG" width="500" height="400">

## Accuracy issues

However, there are some accuracy problems that we have detected during the project:

* We power Arduino Uno externally via a USB port. On the Arduino board, there is a 5V voltage stabilizer, but it is not an ideal stabilizer. According to the datasheet of a stabilizer [LM7805](https://www.sparkfun.com/datasheets/Components/LM7805.pdf), that is used in Arduino, the output voltage has a tolerance range of 4.8 - 5.2V. This affects the accuracy of the ADC block and the measurement itself. We tried to use the bandgap voltage reference (VBG), available in the Arduino board. However, the ADC measurement range would be limited to 1.1V (VBG voltage), which is not enough for decoding the current sensor value (since it has a 2.5V on the output in idle mode). That's why we had to use the internal 5V reference and thus, we worked with a "noisy" IDC block.

* The current sensor, that we used, has a conversion scale of `185mV/A`, and at the same time, ADC with 5V reference has a step of approximately 5 mV. (1024 values/5V). This means, that only 37 ADC steps are inside a relative change for 1A (a 27 mA step in one ADC step). This would not only strictly limit the range of ADC we use(for a 5A sensor, the output voltage within the range 1.575V (322 in ADC) - 3.425V(702 in ADC)), but would limit the minimum current we can measure. Assuming, that ADC has a &plusmn; 12 mV Worst-Case [error](https://forum.allaboutcircuits.com/threads/arduino-uno-adc-accuracy.144622/#:~:text=The%20error%20level%20of%20the,correctly%20read%200%20and%205000mV.), it would add a &plusmn; 65 mA error. As a result, such small currents can't be measured at all and we recommend using the proposed device with a minimum level of `100 mA`.  

* Finally, the sensor overheating (because it has a close to zero, but non-zero resistance) also impacts measurement accuracy. Thus, measuring big currents may be problematic because of this.

## Software description

Our custom [adc.h](https://github.com/dmitrii-semenov/measuring_electricity/blob/main/measuring_electricity/include/adc.h) library is made to help with ADC operations. There, it is possible to choose voltage reference (1.1V VBG or 5V VCC), the prescaler configuration (2,4,8,16,32,64,128), pick the input pin for ADC (analog pins and direct output from onboard temperature sensor) and enable it.

Main source file [main.c](https://github.com/dmitrii-semenov/measuring_electricity/blob/main/measuring_electricity/src/main.c) with the core code.

Regarding other used libraries, we used `timer.h` (for timers and interruptions), `gpio` (pins management), `oled` (OLED display functions), `twi` (library needed for OLED display). 

We also used an internally declared function `Clear_values` to clear the values from the display. However, this function is very specific for our display configuration and it can't be widely used in other projects. That's why we decided NOT to put it into a separate file/library.

In the `main.c` file, we have declared some global variables: 
* `mode`(mode of measurement) - used to switch between different modes of the measured value (0-current,  1-voltage, 2-resistance, 3-capacitance);
* `SW_ena`(button sensor) - this is the variable used for the button control, if the value of `SW_ena` is higher than '0' it means that the button was pressed and the circuit should switch to the next mode if the value is '0' - the button wasn't pressed;
* `Sensor_Off` - this is the offset of the sensor, needed if there is an error(shift) in the ADC or in the sensor and it doesn't hold 2.5V at the output in idle mode(at zero current), then some current can be added to this variable, which will shift the result (used for calibration);
* `ADC_avg` - the variable with the averaged value of the ADC converter(averaged ADC value);
* `Cap_charge` - variable where the total charge of the capacitor is written (for capacity calculation);

Also, there are some parameters, that need to be defined by a user:

* `AVERAGE_FACTOR` - positive integer, determines how many measurements are averaged in the ADC. If it is equal to '1', then there is no averaging, and a string `AVERAGE OFF` is displayed. Otherwise, there is a note `AVERAGE ON`. We highly recommend using at least 10 averages. In that case, the measurements will be delayed (since 10 values need to be measured, and only then result will be displayed), but it will decrease the noise level (but wouldn't eliminate it)

* `REF_V` - reference voltage for calculations, typically 5V (if connected to the Arduino internal voltage reference). This is the voltage at which the capacitor is charged, or to which the resistive load is connected.

* `REF_R` - reference resistance for calculations, used if there is a reference resistance in series with the measured one. Otherwise equals to`0`.

Then we divided the whole code into several major subblocks. 

* In block `ISR(TIMER0_OVF_vect)`, we have performed averaging. Note, that overflows were added to guarantee enough time for ADC to get measured value.

* Block `main(void)` - Here the configuration of the ADC converter with the initial display interface and interruptions setup is located.

* Block `ISR(INT0_vect)` - Button monitor, add '1' to `SW_ena` once the button is pressed.

* Blok `ISR(TIMER1_OVF_vect)` - Here the status of the button is checked. If the button is pressed, the circuit transfers to the next measurement mode by changing the variable `mode` in the range 0 - 3.

* Blok `ISR(ADC_vect)` - The calculations are performed here according to the current measurement mode. After the calculation is done, the result is plotted on the display and the display is activated.  

Here the block diagram of the whole process is presented:

<img src="https://github.com/dmitrii-semenov/measuring_electricity/blob/main/pictures/Block_diag.jpeg" width="200" height="800">

## Instructions

1. The first step is to perform a calibration measurement to change the offset variable `Sensor_Off`;
2. Connect the desired circuit, depending on what type of measurement you want to perform. The proposed circuits are:

`Current Measurement`:

***

`Voltage Measurement`:

***

`Resistance Measurement`:

***

`Capacity Measurement`:
If you would like to measure capacitance, fully discharge the capacitor before connecting to the circuit!

***

3. Set the `AVERAGE_FACTOR` (recommended 10);
4. Set the value of the reference resistor (if used, '0' if not used);
5. Set the value of the reference voltage (5V by default);
6. Turn on the device by uploading code/connecting supply voltage to Arduino;
7. Use the button to select the measurement mode;
8. If the average factor is used, wait till the results appear on the screen (time depends on the value of `AVERAGE_FACTOR`);
9. If you measure a capacitance value, wait till the ADC value equals the idle value (512), which means that the capacitor is fully charged and there is no current flowing through it;

## Video demonstration
Here, the video demonstration is uploaded, where you can see the functionality of our device With both averaging modes and with no averaging.

[Youtube link](https://youtu.be/Z3Xl_Vw_sTI?si=l59vpJ-lhMB0gJPt)

Note: the capacity measurement mode was not ready at the moment of video recording, this was demonstrated personally during the 13th week of the semester.

## References

1. Microchip Techbology Inc. [ATmega328P datasheet](https://www.microchip.com/en-us/product/ATmega328p)
2. Components101. [Introduction to Analog to Digital Converters (ADC Converters)](https://components101.com/articles/analog-to-digital-adc-converters)
3. Sparkfun electronics. [LM7805 datasheet](https://www.sparkfun.com/datasheets/Components/LM7805.pdf)
4. Sparkfun electronics. [ACS712 datasheet](https://www.sparkfun.com/datasheets/BreakoutBoards/0712.pdf)
5. Tomas Fryza. [Useful Git commands](https://github.com/tomas-fryza/digital-electronics-2/wiki/Useful-Git-commands)