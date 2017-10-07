## tzLib Quick-Start Guide

#### The following code represents a minimal implementation of tzLib. 
```
	void Setup() {
			...
			tzSetDefaultZoneID(char*)"America/Chicago");
			tzSetup();
			...
		}
		void Loop() {
			...
			tzLoop();
			...
		}	
```		
#### This methods in the above code perform the following functions:
	
#####     tzSetDefaultZoneID 
			- Records the default time zone ID in memory.
			
#####     tzSetup
            - Reads a block of data from EEPROM memory.
            - Queries the HTTP server to get time zone data.
                - for the time zone registered in EEPROM memory or for the
                  default time zone if no time zone is registered in EEPROM.
            - Writes an updated data to EEPROM, if any new data was received.
            - Configures the device's local time settings.
                - based on query results if valid data was returned.
                - based on EEPROM data if no valid data was returned and
				  EEPROM data existed.
                - to UTC if no data was returned for the default time zone.
				
#####     tzLoop
            - Triggers a DST transition when the scheduled time arrives.
			- Triggers periodic EEPROM refreshes (every 3-9 weeks).
				   
				   
	