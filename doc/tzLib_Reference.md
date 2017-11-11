# tzLib_Reference			tzLib - The Time Zone Library
---
## Required Methods
---

### tzLib.begin() -----------------------------------------------------------------------
Function and Usage:
*	Initializes the tzLib class
*	Searches EEPROM for the tzBlock
	*	records the location of the tzBlock in EEPROM
	*	reads the tzBlock into static memory
					
Declared:	void begin(void);

Example: tzLib.begin();

		
### tzLib.setDefaultZone() --------------------------------------------------------------
 	
Function and Usage:
*	Technically "not required", but this method is almost always used.
*	Specifies the firmware developer's preferred default time zone ID. This overrides the tzLib default "UTC".
*	This call must be placed before tzSetup() in the firmware's setup() section.
*	This Wikipedia page provides a list of valid time zone IDs: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
*	Remember, time zone IDs are case and syntax sensitive.
				
Declared:	void setDefaultZone(char* id); // where id is char[65]
				
Example: tzLib.setDefaultZone("America/New_York");


### ttzLib.setLocalTime() ---------------------------------------------------------------

Function and Usage:
*	Submits an HTTP query to obtain the latest information for the current time zone. 
*	Updates the tzBlock when the HTTP server provides new data.
*	Updates the local device's time zone settings.
*	Must be called in the firmware's Setup() section.
*	Is also called by tzLib.maintainLocalTime() to periodically refresh the tzBlock.
	
Declared:	int setLocalTime(void);
	
Example: tzLib.setLocalTime();
			
Return: EXIT_SUCCESS / EXIT_FAILURE
*	Return values reflect success or failure of the HTTP query and JSON parsing only.
	*	In the FAILURE case, local time would be set based on EEPROM data that could not be validated via the HTTP query.
	*	This method also logs a status message to static memory, for developer troubleshooting, and/or logging. This status message can be obtained using the tzLib.getHttpStatus() method;
				
    
### tzLib.maintainLocalTime() -----------------------------------------------------------

Function and Usage:
*	Automatically adjusts local time settings to or from DST when Time.now() >= the DST transition time stored in EEPROM.
*	Automatically refreshes the data in EEPROM based on a refresh timer.  
	*	The timer is set to refresh every three weeks for time zones with DST transitions, and every 9 weeks for time zones without DST transistions.  
	*	Should a refresh fail, the refresh will be rescheduled based on a retry period which is 11 hours.
	*	Actual refresh/retry times are something less than the stated period to help randomize client requests.)
*	Must be placed in the firmware's Loop() section.

Declared: void maintainLocalTime(void);

Example: tzLib.maintainLocalTime();


---		
## Configuration Methods
---		
### tzLib.changeZone() ------------------------------------------------------------------

Function and Usage:
*	Changes the device's current time zone
*	Often used in a Particle Function to permit the device's time zone to be changed via the web.  Code sample "tzLib301.ino" demonstrates this capability.
*	Remember, time zone IDs are case and syntax sensitive.

				
Declared: void changeZone(char* id);  // where id is char[65]

Example:	tzLib.changezone("America/Denver");


### tzLib.setEepromStartByte() ----------------------------------------------------------

Function and Usage:
*	Allows firmware developers to select the location in EEPROM where tzLib will store the 128-byte tzBlock.
*	If used, this method must be invoked before tzSetup() in the firmware's Setup() section.
*	In the case where a tzBlock exists at another EEPROM location, this command will copy it to the new location, and will overwrite the original location with '0xFF' characters.	This makes is easy for developers to move the tzBlock if/when they need to do so. 
				
Declared: void setEepromStartByte(int sb);	// where sb can be any value between 0 and (EEPROM.length()-128)

Example: tzLib.setEepromStartByte(512);


### tzLib.setHostName() -----------------------------------------------------------------

Function and Usage:
*	Specifies the IP address or DNS name of the HTTP server that host the time zone offset and DNS transition data.
*	Must be placed before (tzSetup() in the firmware's Setup() section. 
				
Declared: 	void setHostName(char* host);	// where host is char[97]

Example: 	tzLib.setHostName("208.85.39.75");


### tzLib.setHostPath() -----------------------------------------------------------------

Function and Usage:
*	Specifies the path to the time zone offset and DNS transition data on the HTTP server.
*	Must be placed before (tzSetup() in the firmware's Setup() section. 
	
Declared: void setHostPath(char* path); 	// where path is char[97] - Note: path MUST begin with a slash "/"

Example: tzLib.setHostPath("/tzLib/getJSON.php");

		
###	tzLib.setHostPort() -----------------------------------------------------------------

Function and Usage:
*	Specifies the server's HTTP port to use
*	Must be placed before (tzSetup() in the firmware's setup() section.
*	When this method is omitted, tzLib will use port 80
	
Declared: 	void setHostPort(uint16_t port);

Example: 	tzLib.setHostPort(8080);	
				

---
Query and Test Methods
---		
### tzLib.getHttpStatus()) --------------------------------------------------------------

Function and Usage:					
*	When tzLib.setLocalTime() runs, it returns EXIT_SUCCESS, or EXIT_FAILURE, and it records a status message to static memory. This method returns that status message to help with troubleshooting and/or logging purposes.
	
Declared: 	char* getHttpStatus(void);

Example:    
```cpp
			If(tzLib.setLocalTime() == EXIT_FAILURE) {
				Serial.println(getHttpStatusMsg());
			}
```			
Return: char* to char[65];
	

### tzLib.getZone() ---------------------------------------------------------------------

Function and Usage:
*	Allows developers to include the time zone ID in logs or notifications.
*	Returns the current time zone ID
		
Declared: 	char* getZone(void); 

Example: 	Serial.println(tzLib.getZone());
		
Return: 	char* to char[65];
		
		
### tzLib.getZoneAbbr() -----------------------------------------------------------------

Function and Usage:
*	Allows developers to include the current time zone abbreviation in logs or notifications.
*	Returns the current time zone abbreviation, which often change with DST transitions.
		
Declared: 	char* getZoneAbbr(void);

Example:	Serial.println(tzLib.getZoneAbbr());
		
Return: 	char* to char[6];
		

### tzLib.setNextTransitionTime() -------------------------------------------------------
FOR DEVELOPMENT AND TEST USE ONLY --- DO NOT USE IN PRODUCTION CODE.

Function and Usage:

This methold provides a way to test the impact of DST transitions on the device and any logs or notifications that it may generate.
*	Allows developers to set the time when the the next DST transition will occur. 
* 	When the transition time arrives, tzLib.maintainLocalTime() will trigger the transition as if it were the official transition time. 
*	This method only works if the currenty selected timezone has a pending DST transition.
* 	To reset local time after the test, call tzLib.setLocalTime(), tzLib.changeZone(), or reboot the device.
	
Declared:	int setNextTransitionTime(time_t time);	// where time is measured in epoch seconds (UTC)

Examples:
*	tzLib.setNextTransitionTime(Time.now() + (60*60)); // trigger a transition in 1 hour.
*	tzLib.setNextTransitionTime(1582165220);	// trigger a transition at a specified time.

Online tools, like the following, can be used to convert date/time to epoch seconds. http://www.onlineconversion.com/unix_time.htm
	
Return:	int (EXIT_SUCCESS / EXIT_FAILURE)
		
					
### tzLib.transitionNow() ---------------------------------------------------------------
FOR DEVELOPMENT AND TEST USE ONLY --- DO NOT USE IN PRODUCTION CODE.

Function and Usage:

This methold provides a way to test the impact of DST transitions on the device and any logs or notifications that it may generate. 
*	Allows developers to perform a pending DST transition immediately. Unlike tzLib.setNextTransitionTime(), this method performs the transition itself, so this method can be executed anywhere in firmware setup() or loop().
*	This method only works if the currenty selected timezone has a pending DST transition.
*	In a DST transition, the following things occur:
	*	the current offset is updated to the transition offset.
	*	the current abbreviation is updated to the transition abbreviation.
	*	the transition data elements are cleared.
	*	Device local time settings are changed to reflect the new offset
* 	To reset local time after the test, call tzLib.setLocalTime(), tzLib.changeZone(), or reboot the device.
*	After a real-world transition, the transition fields will remain cleared until tzLoop() triggers the next EEPROM refresh, or until the device reboots. At that time, data for the following DST transition will be loaded into EEPROM.
		
Declared:	void transitionNow(void);

Example:	tzLib.transitionNow();


###	tzLib.eraseTzEeprom() ---------------------------------------------------------------
FOR DEVELOPMENT AND TEST USE ONLY --- DO NOT USE IN PRODUCTION CODE.
	
Function and Usage:
*	This method erases the tzBlock from EEPROM. Erasing involves overwriting the memory space with '0xFF' characters.
*	Erasing the tzBlock from EEPROM is useful for testing the "new device" scenario.  When there is no tzBlock in EEPROM, tzLib.setLocalTime() will configure the devices's local time using the default time zone ID.
 				
Declared:	void eraseTzEeprom(void);

Example:	tzLib.eraseTzEeprom();

