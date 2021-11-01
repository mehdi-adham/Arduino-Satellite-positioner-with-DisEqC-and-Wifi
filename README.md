# Arduino-Satellite-positioner-with-DisEqC-and-Wifi
The advantage of this device is the possibility of controlling and setting the positioner via Wi-Fi.

With the advancement of smart home appliances and the increasing use of them in today's human life, I thought it would be interesting to design a device that most of us have at home, that in this new design can be used like a smart device

So I chose the **satellite dish positioner**, its main function is to store the position of the satellites in memory and the ability to move to the satellite requested from the receiver.

Now, in this project, I tried to design a board that, in addition to the main task of the positioner, is connected to the network and can be setup and controlled through the network.

 

With this project, we will learn how to use the **ESP chip** and build a smart device instead of using the Wi-Fi module. In the source code section, we will learn to create the **library** we need and create the required classes according to the project objectives.

We connect to the board (web server) and that means sending **http** requests, so we will learn to create libraries that display web pages and use **Ajax** on the web page to instantly display the counter without refreshing the web page.

## hardware
The main part of the board is the use of the ESP8266EX chip. It's a SoC (System on Chip), with IoT (Internet of Things) and deep sleep capabilities. The ESP Controllers from Espressif are becoming a widely popular choice for IoT based designs.

I did not use the ESP8266 module and tried to design with the ESP8266EX hardware design guide at the chip level on the PCB.

The board consists of several main parts, which include:
1. **DiSEqC** Detector
2. **Motor** driver which includes two relays, transistor and optocoupler
3. Dish **motor sensor** counter detection
4. **ESP8266EX chip** that: DiSEqC data processing and according to DiSEqC commands performs the relevant function. (Save satellite position - start the motor - stop the motor, etc.). Connect the board to the Wi-Fi network
5. 24 to 12 V regulator and 3.3 V regulator that supplies power to different parts of the board

**Note:** The board requires a 24 volt 2 amp adapter.

## Source code
The program code is written with the Arduino framework and in the PlatformIO ecosystem. PlatformIO is an open source ecosystem for IoT development with a cross-platform build system, a library manager, and full support for Espressif (ESP8266) development.

 Several new libraries have been used for this project, which I have written myself, including libraries:
- Diseqc.h
- Positioner.h
- wificonfig.h
- EEPROM_ADD.h

We connect to the board (web server) and that means sending http requests, so we create libraries that display web pages and use Ajax on the web page to instantly display the counter without refreshing the web page. libraries written for the web page include:
- index.h
- wificonfigPage.h

After completing the hardware and uploading the firmware on the hardware, it is time to run and test.

For the preliminary test, all you have to do is turn on the board and connect to it via Wi-Fi. With this test, you can evaluate the Wi-Fi connection and control relays to change the direction of the motor.

To do this, we need a 24-volt adapter to turn on the board.

Here are three videos on how to connect the positioner to the dish motor and receiver and test with Wi-Fi and remote control.

https://youtu.be/7OuzMqFP5-o

https://youtu.be/bW-LMHPvRvs

https://youtu.be/6norc0-4YZ8

