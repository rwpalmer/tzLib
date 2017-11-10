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
*	Initializes the tzLib library (MUST BE  THE FIRST tzLib METHOD EXECUTED)
*	Searches EEPROM for the tzBlock where tzLib stores time zone information.
*	Exposes the tzBlock and its location to other tzLib methods. 
	
### tzLib.setDefaultZone()
*	If the developer does not set the default time zone, the default will be "UTC" (Offset 0, no DST).
*	When tzLib starts up, it looks for a time zone ID in EEPROM. If none is found, the default time zone is adopted as the device's time zone. Once validated, the default time zone ID is written to EEPROM. From that point forward, "EEPROM trumps default". This command is therefore ignored after the first boot. You can NOT change the device's time zone by simply changing the default time zone ID.
	*	For changing time zones, refer to the tzLib.changeZone() method and the tzLib.eraseTzEeprom() method.
			
### tzLib.setLocalTime()
*	Queries the HTTP server to get time zone data. Returns type int with values: EXIT_SUCCESS or EXIT_FAILURE to denote the success or failure of the query. After this methods runs, tzLib.getHttpStatus() method can be run to obtain a status message that either reports success, or why the query failed. 
*	Writes to or updates the tzBlock in EEPROM, if the server provides any new time zone data.
*	Configures the device's local time settings based on the latest data available.
			
### tzLib.maintainLocalTime()
*	Triggers a DST transition when the scheduled time arrives
*	Triggers periodic EEPROM refreshes (every 3-9 weeks)
				   
### tzLib.setEepromStartByte()
*	tzLib stores an object called a tzBlock in EEPROM. This object contains the time zone data that tzLib needs to remember when the device reboots. 
*	tzLib will write the tzBlock to EEPROM bytes 0-127 unless otherwise instructed. The tzLib/setEepromStartByte() method conveys that instruction.
* 	In the sample code, tzLib.setEepromStartByte(0); reflects the default setting. 
*	Change the zero (0) to (512) and tzLib will use EEPROM bytes 512 to 639 instead. 
* 	The start byte can range from 0 to (EEPROM.length() - 128), where EEPROM.length() is the size of your device's EEPROM.
*	This, and all other configuration methods should appear before tzLib.setLocalTime().

*	FYI: It is worth noting that when you execute the tzLib.setEepromStartByte() command, you are telling tzLib where you want the tzBlock to be located in EEPROM. If the tzBlock is located in a different location, this method will erase the existing tzBlock, and rewrite it to the new, specified location. The tzBlock location is then updated in static memory so other methods know where the tzBlock now resides. 
	    

	
	