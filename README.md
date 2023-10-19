# Simon-Game-Project---PIC18F4520---Milford-4x20-BKP

This is the C code to the Simon Game project. 
The software was compiled using the XC8 compiler in the MPLAB X (v.5.50) platform. 
The hardware of this project was created and tested on the Proteus (v.8.13) platform. 
It was used a PIC microcontroller (PIC18F4520) and a Milford-4x20-BKP.

The connection between PIC and LCD needs a library to be compiled. You can find this library in microchip's website or in the link below:

https://www.microchip.com/en-us/tools-resources/archives/mplab-ecosystem#MCC2

Scroll down to Peripheral Libraries, in the PIC18F section, find your OS platform, and download the library.

After you've download and installed the library, copy the folders "include" and "source" and paste it in a folder you putted the C code.

In the MPLAB X platform, go to the "project properties" in the the section 
"XC8 compiler" -> "Include directories" -> '...' (add both paths of the files you copied from the libraries and save those configurations)

When you compile the code, it will generate an error in one of the files inside the library due to a "long" convertion, you can just delete this long conversion and compile the code again.
