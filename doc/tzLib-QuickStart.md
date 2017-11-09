## tzLib Quick-Start Guide

	Most firmware developers will only require the four tzLib methods shown in the following
	code sample. However, some users will need to use an additional method to configure where
	tzLib will store it's tzBlock in EEPROM storage.
	
	By default, tzLib stores the 128-byte tzBlock in EEPROM bytes 0 - 127. When these bytes
	are not available, the developer needs to use the tzLib.setEepromStartByte() method to 
	select a new EEPROM location for the tzBlock. For example, if a developer wants tzLib
	to store the tzBlock starting at EEPROM byte 512, he/she would insert the following 
	line somewhere before tzLib.setLocalTime():
		tzLib.setEepromStartByte(512);

	

### A typical / minimal implementation of tzLib 
```
    #include "tzLib.h"
	void Setup() {
		...
		tzLib.begin();
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

#### tzLib.begin()
	* Initializes the tzLib library
	* Searches EEPROM for the tzBlock, records its location, and loads the tzBlock into static memory
	
#### tzLib.setDefaultZone()
    * Records the default time zone ID in static memory
			
#### tzLib.setLocalTime()
* Queries the HTTP server to get time zone data
        * for the time zone registered in EEPROM memory or for the
          default time zone if no time zone is registered in EEPROM
    * Writes an updated data to EEPROM, if any new data is received
	  from the HTTP server
    * Configures the device's local time settings
        * based on query results if valid data was returned
        * based on EEPROM data if no valid data was returned and
          EEPROM data existed
        * to UTC if no data was returned for the default time zone
				
#### tzLib.maintainLocalTime()
    * Triggers a DST transition when the scheduled time arrives
    * Triggers periodic EEPROM refreshes (every 3-9 weeks)
				   

				   

	
	