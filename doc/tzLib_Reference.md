# tzLib_Reference			tzLib - The Time Zone Library

## REQUIRED METHODS ==============================================================

### tzLib.begin() -------------------------------------------------------------
Function and Usage:
*	Initializes the tzLib class
*	Searches EEPROM for the tzBlock
	*	records the location of the tzBlock in EEPROM
	*	reads the tzBlock into static memory
					
Syntax:
	*	tzLib.begin()
			
Return: void
		
### tzLib.setDefaultZone() ------------------------------------------------------
 	
Function and Usage:
*	Technically "not required", but this method is almost always used.
*	Specifies the firmware developer's preferred default time zone ID. This overrides the tzLib default "UTC".
*	This call must be placed before tzSetup() in the firmware's setup() section.
*	This Wikipedia page provides a list of valid time zone IDs: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
				
Syntax: 
*	tzLib.setDefaultZone("<time zone id>");
*	Example: tzLib.setDefaultZone("America/New_York");
			
Return: void


    ttzLib.setLocalTime() -----------------------------------------------------------------

		Function and Usage:
			-	Submits an HTTP query to obtain the latest information for the
                current time zone. 
			- 	Updates the tzBlock when the HTTP server provides new data.
			-	Updates the local device's time zone settings.
			-	Must be called in the firmware's Setup() section.
			-	Is also called by tzLib.maintainLocalTime() to periodically
                refresh the tzBlock.
			
		Syntax: 
			-	tzLib.setLocalTime();
			
		Return: int
                values: EXIT_SUCCESS / EXIT_FAILURE
                The EXIT status reflects success or failure of the HTTP 
                query and JSON parsing only.
				
                In the FAILURE case, local time would be set based on EEPROM
				data that could not be validated via the HTTP query.
				
				In typial usage, a developer might want to display an error
				message in the FAILURE scenario. Such a message can be
				obtained with tzLib.getHttpStatusMsg().
				
    
	tzLib.maintainLocalTime() ------------------------------------------------------------------
		Function and Usage:
			-	Automatically adjusts local time settings to or from DST when 
				Time.now() >= the DST transition time stored in EEPROM.
			-	Automatically refreshes the data in EEPROM based on a refresh 
				timer.  The timer is set to refresh every three weeks for time
				zones with DST transitions, and every 9 weeks for time zones 
				without DST transistions.   Should a refresh fail, the refresh
				will be rescheduled based on a retry period which is 11 hours.
				Actual refresh/retry times are something less than the stated 
				period to help randomize clients requests.)
			-	Must be placed in the firmware's Loop() section.

		Syntax: 
			-	tzLib.maintainLocalTime();
			
		Return: void

		
CONFIGURATION METHODS ========================================================
		
    tzLib.changeZone() --------------------------------------------------------
        Function and Usage:
            -	Changes the device's current time zone
            -	Often used in a Particle Function to permit the device's time
                zone to be changed via the web. 
            	Code sample tzLib301.ino demonstrates this capability.
				
        Syntax: tzLib.changeZone("<time zone id>);
            -	Example:tzLib.changezone("America/Denver");

        Return: void
		
		

    tzLib.setEepromStartByte() ------------------------------------------------

        Function and Usage:
            -	Allows firmware developers to select the location in EEPROM
                where tzLib will store the 128-byte tzBlock.
            -   If used, this method must be invoked before tzSetup() in the 
                firmware's Setup() section.
            -   In the case where a tzBlock exists at another EEPROM location, 
                this command will copy it to the new location, and will
                overwrite the original location with '0xFF' characters.	This
                makes is easy for developers to move the tzBlock if/when they
				need to do so. 

		Syntax: 
            -	tzLib.setEepromStartByte(<location>);
                <location> type is int
                <location> valid range is  0 to (EEPROM.length()-128)
            -   Example: tzLib.setEepromStartByte(512);

        Return: void
		

    tzLib.setHostName() -------------------------------------------------------
        Function and Usage:
            -	Specifies the IP address or DNS name of the HTTP server that
                host the time zone offset and DNS transition data.
            -	Must be placed before (tzSetup() in the firmware's Setup() 
                section. 
				
        Syntax: 
            -	tzSetHostName(("<server address>");
            -   Example: tzSetHostName("208.85.39.75");
				
        Return: void


    tzLib.setHostPath() -----------------------------------------------------------
		Function and Usage:
			-	Specifies the path to the time zone offset and DNS transition
				data on the HTTP server.
			-	Must be placed before (tzSetup() in the firmware's Setup() 
				section. 
	
 		Syntax: 
			-	tzSetHostPath("<path>");
				( <path> MUST begin with a slash "/" )
			-	Example: tzLib.setHostPath("/tzLib/getJSON.php");
			
		Return: void

		
	tzLib.setHostPort() -----------------------------------------------------------
		Function and Usage:
			-	Specifies the server's HTTP port to use
			-	Must be placed before (tzSetup() in the firmware's Setup() 
				section.
            -   When this method is omitted, tzLib will use port 80
	
 		Syntax: 
			-	tzLib.setHostPort(<port number>);
            -   Example: tzLib.setHostPort(8080);	
				
		Return: void



QUERY AND TEST METHODS ========================================================
		
	tzLib.getHttpStatus()) -------------------------------------------------

		Function and Usage:					
			-	When tzLib.setLocalTime() runs, it returns EXIT_SUCCESS, or 
				EXIT_FAILURE, and it records a status message to static memory.
				This function returns the status message.
	
		Syntax: 
			-	tzLib.getHttpStatus();
            -   Example: If(tzLib.setLocalTime() == EXIT_FAILURE) {
                             Serial.println(getHttpStatusMsg());
                         }
			
		Return: char* to char[65];
	
    tzLib.getZone() -----------------------------------------------------------

        Function and Usage:
            -  returns the current time zone ID
		
        Syntax: getZone();
		
		Return: char* to char[65];
		
		
    tzLib.getZoneAbbr() -----------------------------------------------------------

        Function and Usage:
            -  returns the current time zone abbreviation which often changes with
               with DST transitions.
		
        Syntax: getZone();
		
		Return: char* to char[65];
		

	tzLib.setNextTransitionTime() ---------------------------------------------
	
		Function and Usage:
            FOR DEVELOPMENT AND TEST USE ONLY --- DO NOT USE IN PRODUCTION CODE.
			-	Allows developers to perform DST transitions for testing. 
			-	This method only works if the currenty selected timezone has
				a pending DST transition.
			-	Test DST transitions can be reversed by a device reboot or a
				call to Setup().
			-	In a DST transition, the following things occur:
					-	currentOffset is changed to transitionOffset
					-	transitionTime and transitionOffset are zeroed
					-	Device local time settings are changed to reflect the
						the new offset. 
			-	After a real-world transition, the transition time and offset
				fields will remain zero until tzLoop() triggers the next 
				EEPROM refresh, or until the device reboots. At that time,
				data for the next DST transition should be loaded into EEPROM.
		
		Syntax:
            -	tzSetNextTransitionTime((time_t)<epoch seconds UCT>);
            -   Example: to trigger a transition in 1 hour ...
                         tzLib.setNextTransitionTime(Time.now() + (60 * 60));
        Return:	void
		
					
    tzLib.transitionNow() -----------------------------------------------------
        Function and Usage:
            FOR DEVELOPMENT AND TEST USE ONLY --- DO NOT USE IN PRODUCTION CODE.
            -   Shorthand for tzLib.setNextTransitontime(Time.now());
		
        Syntax:
            -   tzLib.transitionNow();
			
        Return: void


    tzLib.eraseTzEeprom() -----------------------------------------------------
	
		Function and Usage:
            FOR DEVELOPMENT AND TEST USE ONLY --- DO NOT USE IN PRODUCTION CODE.
            -   This method erases the tzBlock that tzLib stores in EEPROM. 
                 - the tzBlock is overwritten with '0xFF' characters. 
                As a result,
                 - tzLib will forget the configured time zone.
                 - tzLib will forget the data required to configure local time
                   and to perform DST transitions.
            This method is provided to help developers test the 'new device'
            scenario.
 				
		Syntax:
			-	tzLib.eraseTzEeprom();
		
		Return:	void
