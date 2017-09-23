README

tzLib_101 is provided primarily as a code sample to show firmware
developers how to implement the fundimental tzLib elements. Sample output is
included as comments at the bottom of the source code.

tzLib_101 demonstrates the basics ... How to: 
	- 	Set the default time zone with the tzSetDefaultZoneId method
	- 	Configure the HTTP Host with the tzSetHttpHost and
		tzSetHttpPath methods
	- 	Use the tzSetup method to get data from the HTTP Server and to
		configure the device's settings related to local time
	-	Use the tzLoop method to perform DST transitions at the 
		appropriate time

The methods listed above are the only ones most applications will require.

tzLib_101 also demonstrates how to use the tzGetInfo method to access
the data tzLib stores in EEPROM. While that is something few applications
will require, some may find it useful.

IF YOU CHOOSE TO RUN THIS DEMO ... PLEASE READ THIS:
tzLib_101 may display data for a timezone that is NOT the one specified
in the tzSetDefaultZoneId method. If this happens, it is because the 
tzSetup method found the displayed time zone ID in EEPROM. (When actual data
is found, default data is ignored.) If you want tzSetup to use the time zone
specified by the tzSetDefaultZoneId statement, you can erase the data
in EEPROM. That can be done by running the tzWipeEEPROM method before running
the tzSetDefaultZoneId method in your firmware.
	(Syntax is simply: tzWipeEEPROM();). 

tzWipeEEPROM is specifically designed to help firmware developers test their
default (OOB) scenarios. This method must NOT be used in final code. 


