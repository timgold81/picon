#picon
"Picon" is an abbriviation of PI console. Picon is an application that implements serial console using raspberry pi.

#Operation
The program is starts on user connecting to the RPi. Depends on the program port mapping in the file picon.conf, the program opens minicom to some serial connected to the RPI by USB.
For example, by connecting to the PI using ssh on port 2000, the user connected to serial port on USB0. By connecting to port 2001 the user is connected to serial port on USB1. This "port mapping" is configurable via the file picon.conf. By connecting to port 22 the user can configure users, port mapping and other options.


#Features
Automatic portmapping of ssh to minicom
Configuration of Users, Port mapping

#Featuers of the Raspberry PI
Small dimentions - device as small as credit card
Low power supply - 5v, 1A
Advantage of open linux system

#Road Map
In the future the following features will be developed:

Simplification of TACACS configuration
Interactive menu
Case adaptable to Data Center Racks

