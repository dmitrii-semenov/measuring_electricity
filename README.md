# Project in C

Topic: Measuring Electricity

### Team members

* Dmitrii Semenov (responsible for debugging, C programing, hardware description)
* Roman Lunin (responsible C programing, github design, software description)
* Vladislav Karmanov (responsible for nothing)

## Theoretical description and explanation

To implement our project, we needed libraries that we used in computer classes: `timer.h`, `gpio`, `oled`, `twi` and we needed to write new libraries with which we could correctly program the design of the display itself to display measurements and program the pins from which we get the main information. 

In order to make the design of displaying the results on the display, we used a library `code_functions` that consists of two parts, the first part `code_functions.h` contains just the names of the functions used, and the second part `code_functions.c` contains the main code with information about how data is written to registers and configuration takes place, with the help of which we can subsequently display the values or clear the display before subsequent measurement, it cleans only the positions where the measurement figures of our quantities are located and does not touch the rest of the interface, which must be permanently stored on our displays.




## Hardware description of the demo application

In the main file, we have global variables involved: 
* `mode`(mode of measurement) - the first variable is used to switch the mode of the measured value (0-current,  1-voltage, 2-resistance, 3-capacitance);
* `SW_ena`(button sensor) - this is the signal used for the button, if the `SW_ena` is in one(1) it means that the button is pressed, if it is at zero(0) it means that the button is not pressed;
* `Sensor_Off` - this is the offset of the sensor, if we assume that there is some kind of error in the ADC or in the sensor and it does not output 2.5V at zero current, then some current can be clamped as this variable, which will work as an offset(used for calibration);
* `ADC_avg` - the variable into which the averaged value of the ADC converter is mapped(averaged ADC value);

* `AVERAGE_FACTOR` - determines how many measurements we will average in the adc. If it is equal to 1, then there is no averaging, every time the adc outputs a new value, we use it, but the adc has a lot of noise, and therefore if 1 is used, the output value jumps very much. Therefore, it is recommended to use 10. First, 10 values will be measured ,the average value will be given out of them , and so on over and over again;

* `REF_V` - reference voltage for calculations;
* `REF_R` - reference resistance for calculations;


## Accuracy issues

* Firstly, due to the fact that the arduino can be powered externally and from the USB port, there is a 5V voltage stabilizer, but it does not cope well with its function and therefore the voltage can jump by +- 250 mV, and it follows that the voltage jumps on the sensor itself and this makes an error in measurement of quantities. Secondly, the accuracy of the adu drive deteriorates because if the voltage jumps, then the entire scale jumps. As a result, 5V are unstable, and this means that the sensor will have its middle shifted, that is, it will output some other value at zero current and because of this our device will not work correctly. Theoretically, we could solve this problem with the help of MSG, but this will not work because of the structure of the sensor, because we need to read 2.5V and more.




## Software description

Put flowcharts of your algorithm(s) and direct links to source files in `src` or `lib` folders.

## Instructions

Write an instruction manual for your application, including photos and a link to a short app video.

## References

1. Write your text here.
2. ...