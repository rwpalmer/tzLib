# tzLib Quick-Start Guide


The code listed below represents a typical implementation of tzLib. Information about the functionality and usage of each method appear below the code. Further information about the methods can be found in tzLib_Reference.md.

If data is currently stored in your device's EEPROM, please pay special attention to the tzLib.setEepromStartByte() method to assure tzLib does not overwrite that data.

### Sample Code:
```
    #include "tzLib.h"
	void Setup() {
		...
		tzLib.begin();
		tzLib.setEepromStartByte(0);
		tzLib.setDefaultzone("America/Chicago");
		tzLib.setLocalTime();
		...
	}
	void Loop() {
		...
		tzLib.maintainLocalTime();
		...
	}	
```		
### The functionality provided by the above code

### tzLib.begin()
*	Initializes the tzLib library (MUST be executed before ANY other tzLib methods are run.
*	Searches EEPROM for the tzBlock where tzLib stores time zone information.
*	Exposes the tzBlock and its location to other tzLib methods. 
	
### tzLib.setDefaultZone()
*	This method should be executed before tzLib.setLocalTime().
*	If the developer does not set the default time zone, tzLib will default to "UTC" (Offset 0, no DST).
*	When tzLib starts up, it looks for a time zone ID in EEPROM. If none is found, the default time zone is adopted as the device's time zone. Once validated, the default time zone ID is written to EEPROM. From that point forward, "EEPROM trumps default". This command is therefore ignored after the first boot. While you can initially set the devices timezone with this method, it can not be used to change the timezone once a timezone has been registered in EEPROM.
	*	For changing time zones, refer to the tzLib.changeZone() method and the tzLib.eraseTzEeprom() method.
			
### tzLib.setLocalTime()
*	Must run in the firmware setup();
*	Queries the HTTP server to get time zone data. Returns type int with values: EXIT_SUCCESS or EXIT_FAILURE to denote the success or failure of the query. After this methods runs, tzLib.getHttpStatus() method can be run to obtain a status message that either reports success, or why the query failed. 
*	Updates the tzBlock in EEPROM whenever the server provides new data.
*	Configures the device's local time settings based on the latest data available.
			
### tzLib.maintainLocalTime()
*	Must run in the firmware loop().
*	Triggers a DST transition when the scheduled time arrives
*	Triggers periodic EEPROM refreshes (every 3-9 weeks)
				   
### tzLib.setEepromStartByte()
tzLib stores an object called a tzBlock in EEPROM. This object contains the time zone data that tzLib needs to remember when the device reboots.  tzLib will write the tzBlock to EEPROM bytes 0-127 unless otherwise instructed. The tzLib.setEepromStartByte() method conveys that instruction.
*	This method should be executed before tzLib.setLocalTime().
* 	In the sample code, the statement "tzLib.setEepromStartByte(0);" reflects the default setting, so ...
	*	The statement can be removed if the default setting is desired.
	*	The zero (0) can be changed to another value if the tzBlock needs to be stored in a different EEPROM location.
		*	For example, change the zero (0) to (512) and tzLib will store the tzBlock in EEPROM bytes 512 to 639. 
		*	Valid locations range from 0 to (EEPROM.length() - 128), where EEPROM.length() is the size of your device's EEPROM.

FYI: It is worth noting that when you execute the tzLib.setEepromStartByte() command, you are telling tzLib where you want the tzBlock to be located in EEPROM. If the tzBlock is located in a different location, this method will erase the existing tzBlock, and rewrite it to the new, specified location. The tzBlock location is then updated in static memory so other methods know where the tzBlock now resides. 
	    

	
	