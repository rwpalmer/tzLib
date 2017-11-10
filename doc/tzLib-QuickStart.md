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
		tzLib.setDefaultzone("America/New_York");
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
*	In the sample code, the tzLib.setDefaultzone() statement sets the default time zone ID to "America/New_York". 
	*   Assuming this is not your preference, this Wikipedia page provides a list of the valid time zone IDs that can be used:          https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
	*	Case and syntax matter: "america/new_york" would be invalid, as would "America/NewYork".
*	The default time zone id will be adopted as the device's current time zone ID once tzLib.setLocalTime() determines that it is valid and stores the ID in EEPROM.  From that point forward, "EEPROM trumps default", so the tzLib.setDefaultZone() method will be ignored on subsequent reboots ... unless the tzBlock is erased from EEPROM.
*	The above logic means that the timezone ID can be changed in EEPROM, without fear that the current timezone would revert back to the default each time  the device is rebooted. 
	* The current time zone ID can be changed with the tzLib.changeZone() command. This normally runs in a Particle Function that is executed via the web ... permitting the timezone to be changed without the need for a firmware change and compile.
	* For testing purposes, the tzLib.eraseTzBlock() method can be used to erase the tzBlock from memory ... so the tzLib.setDefaultZone() method can again be used to set the current time zone ID. 

	
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
	    

	
	