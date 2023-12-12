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

**Measuring of capacitance:** Capacitance is measured according to its definition. For a capacitor, the capacitance equals the charge divided by voltage. Thus, if we apply a known voltage (5V `REF_V`) to the **discharged!** capacitor, the integral of its current in the time domain equals its charge. We know that during the charging process, the current through the capacitor decreases exponentially, so after the measurement is started, a user should wait an appropriate amount of time for current stabilization. The code adjusts the measured value is well, displaying it in `uF`, `mF` or `F`. the total equation is:

$$C = \frac{1}{V_{ref}} \cdot \sum_{t=0}^{t=n}I_n \cdot \Delta_t$$

## Accuracy issues

* Firstly, due to the fact that the arduino can be powered externally and from the USB port, there is a 5V voltage stabilizer, but it does not cope well with its function and therefore the voltage can jump by +- 250 mV, and it follows that the voltage jumps on the sensor itself and this makes an error in measurement of quantities. Secondly, the accuracy of the adu drive deteriorates because if the voltage jumps, then the entire scale jumps. As a result, 5V are unstable, and this means that the sensor will have its middle shifted, that is, it will output some other value at zero current and because of this our device will not work correctly. Theoretically, we could solve this problem with the help of bandgap(VBG), but this will not work because of the structure of the sensor, because we need to read 2.5V and more.

Circuit of a `bandgap` reference:

<img src="https://github.com/dmitrii-semenov/measuring_electricity/blob/main/pictures/Bandgap-reference.svg.png" width="300" height="300">


* The sensor has a conversion scale of `185mV/A`, and at the same time we use ADC, which has only `1024 values/5V`, this means that the ADC has a division scale of only 5mV, this is a fairly small value, and if there is some noise or voltage surges, then our ADC signal will be very noisy because of this, he jumps all the time and we can't exactly measure something.

* Also, one of the reasons for inaccuracies in the measurement may be overheating of the sensor and other components, but we do not take this into account and therefore this may affect the quality of the measurements.

## Software description

**adc:**

* [adc.h](https://github.com/dmitrii-semenov/measuring_electricity/blob/main/measuring_electricity/lib/adc/adc.h)

**main.c:**

* [main.c file](https://github.com/dmitrii-semenov/measuring_electricity/blob/main/measuring_electricity/src/main.c)

To implement our project, we needed libraries that we used in computer classes: `timer.h`, `gpio`, `oled`, `twi` and we needed to write new libraries with which we could correctly program the design of the display itself to display measurements and program the pins from which we get the main information. 

In order to make the design of displaying the results on the display, we used a library `code_functions` that consists of two parts, the first part `code_functions.h` contains just the names of the functions used, and the second part `code_functions.c` contains the main code with information about how data is written to registers and configuration takes place, with the help of which we can subsequently display the values or clear the display before subsequent measurement, it cleans only the positions where the measurement figures of our quantities are located and does not touch the rest of the interface, which must be permanently stored on our displays.

Since we chose the way to solve our project using adc, in order to configure the registers and the adc function, we needed to make a new library `adc` where the registers for adc are written and where the names of the functions that we will use are simply written. The library also consists of 2 parts, in `adc.h `part the names of the functions are simply spelled out, and in the `adc.c` part registers are added to the functions(Library with custom ADC functions).

In the `main.c` file, we have global variables involved: 
* `mode`(mode of measurement) - the first variable is used to switch the mode of the measured value (0-current,  1-voltage, 2-resistance, 3-capacitance);
* `SW_ena`(button sensor) - this is the signal used for the button, if the `SW_ena` is in one(1) it means that the button is pressed, if it is at zero(0) it means that the button is not pressed;
* `Sensor_Off` - this is the offset of the sensor, if we assume that there is some kind of error in the ADC or in the sensor and it does not output 2.5V at zero current, then some current can be clamped as this variable, which will work as an offset(used for calibration);
* `ADC_avg` - the variable into which the averaged value of the ADC converter is mapped(averaged ADC value);

* `AVERAGE_FACTOR` - determines how many measurements we will average in the adc. If it is equal to 1, then there is no averaging, every time the adc outputs a new value, we use it, but the adc has a lot of noise, and therefore if 1 is used, the output value jumps very much. Therefore, it is recommended to use 10. First, 10 values will be measured ,the average value will be given out of them , and so on over and over again;

* `REF_V` - reference voltage for calculations;
* `REF_R` - reference resistance for calculations;


Also, due to the fact that the ADC is too noisy in block `ISR(TIMER0_OVF_vect)`, we have done averaging, which will stabilize the values very well, this will greatly help in improving the measurement accuracy. We did this in such a way that the user can set himself what amount of averaging he wants (how much he wants to average the value).

* blok `int main(void)` - this is the setting of the ADC converter, so that there is a correct reference, so that everything is correctly active, the input is selected (configuration of the adc)

* command `GPIO_mode_input_pullup(&DDRD, SW)` - sets the pin to which the button is connected, the initial value is 1. If you connect the button without this command, then it works poorly, if you press it, it fixes that the button is pressed, and if you release it, it does not fix that the button is started. This is done because the ground appears on the pin in some way, and then he cannot identify that the button has been pressed. But if we make a command, then the pin wants to see 1, and when we press the button, 1 appears there and when the button is pressed, then there is no longer 1, and he already understands that the button is not pressed.**(Dima posmotri nado li eto ili naxyi ne nado)**

* blok `ISR(INT0_vect)` - button monitor

* blok `ISR(TIMER0_OVF_vect)` - we read the value from the adc once every 50ms. We read one value, and we increase number of average, we read the value and add to `ADC_avg_internal` this value divided by the number of averages. And as soon as we reach the required number of averages, which is determined by the user, then we skip the signal ADC internal to ADC avg.

* blok `ISR(TIMER1_OVF_vect)` - it is responsible for the button, it is triggered once every third of a second. He looks at whether the button has been pressed at least once during this time. If yes, then it switches the mode to the next measurement mode.
* blok `ISR(ADC_vect)` - the calculation itself is already happening here. As soon as an ADC signal appears, we convert it to voltage. For calculations, we use the average value, then we count the current, then we count the resistance and capacitance.

## Instructions

Write an instruction manual for your application, including photos and a link to a short app video.
1. The first step is to perform a calibration measurement in order to understand the offset and set it in the program;
2. Before starting the measurement, you need to check which voltage you have;
3. Set the `AVERAGE_FACTOR`;
4. Set the value of the reference resistor;
5. Connect the desired circuit, depending on what value you want to measure (current, voltage, resistance, capacitance);
6. Turn on the device;
7. Use the button to select the mode to measure the desired value;
8. Now wait a bit , the measurement is being carried out;

## References

1. Write your text here.
2. ...