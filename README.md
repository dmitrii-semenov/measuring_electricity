# Project in C

Topic: Measuring Electricity

### Team members

* Dmitrii Semenov (responsible for debugging, C programing, hardware description)
* Roman Lunin (responsible C programing, github design, software description)
* Vladislav Karmanov (responsible for nothing)

## Theoretical description and explanation

After learning the topic of our project, after much thought, we decided to invent a device that can measure electrical quantities. To implement our idea, we needed a current sensor, which we were given initially. We wanted to make a decent interface so that measurements could be conveniently and intuitively controlled, so we used the display and the button to switch modes. After that, we had a question about implementation, how can we do this? The only option was to use `ADC`(Analog digital converter), since we cannot use an `analogRead`. We studied how it works, looked at which registers are used and did it in such a way as to read from the input analog pin (in our case A0), and subsequently process them in some way. After that, we implemented blocks to control the button, to switch modes. We have made a FSM(Finite State Machine) that switches between 4 modes. Each mode corresponds to a measurement of one value.

Our device can measure current and voltage, which is the most important thing for a battery, moreover, we decided to increase the area of use of the device and we eventually got a kind of multimeter, because we can measure the resistance of components, we can measure capacitors, we can measure currents and voltages, and in the future it is possible to add a function for measurements of transistors . With the help of the sensor, we will be able to determine where the non-zero current flows and in this way check the operability of the transistors.

Also, the advantage of our project is that we have made a library `adc` that can be used not only in our project, but also in others. Which greatly expand the range of use and usefulness.




## Hardware description of the demo application

Measuring of current: we consider the current so that we know that the sensor outputs 2.5V, which means zero current , so we subtract 
2.5V and divide by 185mV, this is the price of division, which we learned from the datasheet 185mV/1A. And as the last step, we add an offset 

`current_meas = ((value_avg*1000.00)-2500.00)/185.00 + Sensor_Off;`;

Measuring of voltage:

Measuring of resistance:

Measuring of capacitance:




## Accuracy issues

* Firstly, due to the fact that the arduino can be powered externally and from the USB port, there is a 5V voltage stabilizer, but it does not cope well with its function and therefore the voltage can jump by +- 250 mV, and it follows that the voltage jumps on the sensor itself and this makes an error in measurement of quantities. Secondly, the accuracy of the adu drive deteriorates because if the voltage jumps, then the entire scale jumps. As a result, 5V are unstable, and this means that the sensor will have its middle shifted, that is, it will output some other value at zero current and because of this our device will not work correctly. Theoretically, we could solve this problem with the help of bandgap(VBG), but this will not work because of the structure of the sensor, because we need to read 2.5V and more.

Circuit of a `bandgap` reference:

<img src="https://github.com/dmitrii-semenov/measuring_electricity/blob/main/pictures/Bandgap-reference.svg.png" width="300" height="300">


* The sensor has a conversion scale of `185mV/A`, and at the same time we use ADC, which has only `1024 values/5V`, this means that the ADC has a division scale of only 5mV, this is a fairly small value, and if there is some noise or voltage surges, then our ADC signal will be very noisy because of this, he jumps all the time and we can't exactly measure something.

* Also, one of the reasons for inaccuracies in the measurement may be overheating of the sensor and other components, but we do not take this into account and therefore this may affect the quality of the measurements.

## Software description

Put flowcharts of your algorithm(s) and direct links to source files in `src` or `lib` folders.

**code_functions:**

* [code_functions.c](https://github.com/dmitrii-semenov/measuring_electricity/blob/main/measuring_electricity/lib/code_functions/code_functions.c)

* [code_functions.h](https://github.com/dmitrii-semenov/measuring_electricity/blob/main/measuring_electricity/lib/code_functions/code_functions.h)


**adc:**

* [adc.c](https://github.com/dmitrii-semenov/measuring_electricity/blob/main/measuring_electricity/lib/adc/adc.c)

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

* command `GPIO_mode_input_pullup(&DDRD, SW)` - sets the pin to which the button is connected, the initial value is 1. If you connect the button without this command, then it works poorly, if you press it, it fixes that the button is pressed, and if you release it, it does not fix that the button is started. This is done because the ground appears on the pin in some way, and then he cannot identify that the button has been pressed. But if we make a command, then the pin wants to see 1, and when we press the button, 1 appears there and when the button is pressed, then there is no longer 1, and he already understands that the button is not pressed.(Dima chekni nujno li eto ili naxyi)

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