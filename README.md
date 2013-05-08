
About RL78G14 Exosite Cloud Demo
========================================
This project is an IAR Renesas RL78G14 project that using Novatel CNN0301-10 cellular radio to demo Exosite cloud activatation,
and cloud connectivity to send and receive data to/from the cloud by using Exosite's Cloud Data Platform.

The project functionality includes:

1. New device activation and save cik to EEPROM.
2. Period send data to draw triangle wave to Exosite cloud.
3. Send adc value and temp to Exosite cloud.
4. Read data source on Exosite cloud.

License is BSD, Copyright 2012, Exosite LLC (see LICENSE file)

Tested and developed ith IAR for RL78 30-day evaluation 1.20.4.

Quick Start
========================================
1. Download and install [IAR Embedded Workbench](http://supp.iar.com/Download/SW/?item=EWRL78-EVAL) for Renesas RL78 30-day evaluation
2. Add a new device to your [Exosite Portal](http://renesas.exosite.com) with the MEID shown on the Novatel module.
    - The device type will be (YRDKRL78G14 with Novatel CDMA)
3. Download project repository as a zip file.
4. Extract the project and open it with IAR.
5. Confirm SW5.2(SW5, switch 2) is OFF(down) then plug in the USB cable to your PC
6. Compile the project and download the program to the board
7. Flip switch 5.2(up) to the on position
7. If this is the first time the modem has been used, you will need to activate it on the cellualar network to do this:
    - Press and hold switch 1, and reset the board.
    - The LCD will show the activation process.
    - The LCD will show "Network Activation Successful" if the modem is successfully activated.
8. Now that your modem is activated, reset the board with out any buttons pressed  
9. The device should now turn on and connect to the Exosite platform.
9. When connected, the device will periodically send the folowing sensor values to Exosite
    - Potentiometer
    - Temperature
    - Ping
        - The ping is a value that starts at zero and increments every time sensor values are reported
10. The device also reads data from the Exosite that tells the device if it should turn the LEDs on, or off.

Error messages
==============
Possible error messages while attempting to communicate with Exosite.

|Error Msg         | Possible cause                                                                                              
|----------------- | ---------------------------------------------------------------
| Connect Err      | Problem getting modem to open socket                                                                        
| Conn err         | Response for activation didn't matck expected response length
| Dev Not Enab     | Received a 409 response when activating with platform, and there isn't a CIK stored in NVM
| R_W Error        | Received a 401 response when activating with Exosite
| Unknown: 1       | This should only happen if data corruption, in memory, occurs. In this case we have no idea what happened.

========================================
Release Info
========================================

* **v1.0**:

    - 5-03-2013 -- Initial Release of Novatel branch
 


