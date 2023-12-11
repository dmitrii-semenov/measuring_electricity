# Project in C

Topic: Measuring Electricity

### Team members

* Dmitrii Semenov (responsible for debugging, C programing, hardware description)
* Roman Lunin (responsible C programing, github design, software description)
* Vladislav Karmanov (responsible for nothing)

## Theoretical description and explanation

To implement our project, we needed libraries that we used in computer classes: 'timer.h', 'gpio', 'oled', 'twi' and we needed to write new libraries with which we could correctly program the design of the display itself to display measurements and program the pins from which we get the main information. 
In order to make the design of displaying the results on the display, we used a library 'code_functions' that consists of two parts, the first part contains just the names of the functions used, and the second part contains the main code with which we display the values or clear the display before subsequent measurement.


## Hardware description of the demo application

Insert descriptive text and schematic(s) of your implementation.

## Accuracy issues

* Firstly, due to the fact that the arduino can be powered externally and from the USB port, there is a 5V voltage stabilizer, but it does not cope well with its function and therefore the voltage can jump by +- 250 mV, and it follows that the voltage jumps on the sensor itself and this makes an error in measurement of quantities. Secondly, the accuracy of the adu drive deteriorates because if the voltage jumps, then the entire scale jumps.



## Software description

Put flowcharts of your algorithm(s) and direct links to source files in `src` or `lib` folders.

## Instructions

Write an instruction manual for your application, including photos and a link to a short app video.

## References

1. Write your text here.
2. ...