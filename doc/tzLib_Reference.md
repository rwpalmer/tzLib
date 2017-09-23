tzLib_Reference			tzLib - The Time Zone Library

	Configures a device's local time zone offset and performs DST 
	transitions when an IANA defined time zone ID is provided.
	
		Who is IANA:
			In additon to administering IP addresses and domain names, the 
			Internet Assigned Numbers Authority (IANA) maintains the 
			time-zone database that is used to determine local time settings
			by the Communications and IT industries world-wide. IANA updates
			the database periodically because local time and DST settings are
			determined by the government bodies that oversee geographic areas,
			and those governments make changes from time to time.

		What is an IANA time zone ID?
			Every database needs a key, and the primary key to the IANA time 
			zone database is called a "time zone ID". Examples: Europe/Paris,
			America/New_York, Asia/Singapore, ...
			
	By default, tzLib uses the IANA time zone ID "UCT" (offset = 0, no DST).
		- 	tzLib methods allow firmware developers to select a different
			default.
		-	tzLib methods can allow device users to override the firmware
			developers default if the developer provides them with a menu or 
			map-based time zone selection tool. A number of these are 
			available on GitHub and elsewhere. 
		-	Geopositioning software can also use tzLib methods to reconfigure 
			device settings	as mobile devices move from one time zone to 
			another.
	

REQUIRED METHODS ==============================================================

    tzSetup() -----------------------------------------------------------------

		Function and Usage:
			-	Reads the time zone information stored in EEPROM. 
			-	Downloads the latest time zone offset and DST transiton data
				for the configured timezone.
			- 	Updates EEPROM when the web provides new informaton.
			-	Updates the local device's time zone settings.
			-	Must be called in the firmware's Setup() section.
			-	May be called in a Particle function to change time zone.
			
		Syntax: 
			-	tzSetup(); <-- when used in tzSetup().
			-	tzSetup((char*)"<time zone id>");  <-- to change time zone IDs
			
		Return: None ... See the tzGetSetupReturnMsg() method.
	
    
	tzLoop() ------------------------------------------------------------------
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
			-	tzLoop();
			
		Return: None

		
CONFIGURATION METHODS ========================================================
		
	tzSetDefaultZoneId() ------------------------------------------------------
 	
		Function and Usage:
			-	Specifies the firmware developer's preferred default time zone
				ID. This overrides the tzLib default "UCT".
			-	This call must be placed before tzSetup() in the firmware's
				Setup() section.

		Syntax: 
			-	tzSetDefaultZoneId((char*)"<time zone id>");
			
		Return: None


    tzSetEepromLocation() -----------------------------------------------------

		Function and Usage:
			-	Allows firmware developers to select the location in EEPROM
				where tzLib will store time zone data.
			-	If used, this method must be invoked before tzSetup() in the 
				firmware's Setup() section, and it must stay there so tzLib 
				knows where it can find its data after the device reboots. 
			-	By default tzLib stores data beginning at EEPROM location 0.
			-	The current version of the tzInfo_t struct (version 01.00.00a) 
				requires 92 bytes of EEPROM storage.
				storage.
			-	This method does no checking to validate that the location
				provided is within the scope of EEPROM storage, or that the
				location does conflict with storage used by other applications.
				So, please use caution when selecting an EEPROM location.
			-	If this method is called when valid time zone data exists
				in the default location, this command will move that data to
				the new location, and data will be erased from the default
				location.
			-	If valid time zone data exists at a non-default location, that
				data can also be moved if this command is issued twice. 
				For example, assume we want to move the data from location
				120 to 330. We could issue the following commands:
					tzSetEepromLocation(120);
					tzSetEepromLocation(330);
				When the first command runs, there is no valid data at the
				default location, so (120) effectively becomes the new default. 
				The second command runs, it finds data at the default (120),
				and that data would be moved to the new location (330).

		Syntax: 
			-	tzSetEepromLocation(<location>); <-- location is type 'int'.
			
		Return: None
		

	tzSetHttpHost() -----------------------------------------------------------
		Function and Usage:
			-	Specifies the IP address or DNS name of the HTTP server that
				host the time zone offset and DNS transition data.
			-	Must be placed before (tzSetup() in the firmware's Setup() 
				section. 
				
 		Syntax: 
			-	tzSetHttpHost((char*)"<server address>");
			
		Return: None


	tzSetHttpPath() -----------------------------------------------------------
		Function and Usage:
			-	Specifies the path to the time zone offset and DNS transition
				data on the HTTP server.
			-	Must be placed before (tzSetup() in the firmware's Setup() 
				section. 
	
 		Syntax: 
			-	tzSetHttpPath((char*)"<path>");
			
		Return: None


QUERY AND TEST METHODS ========================================================
		
	tzGetInfo() ---------------------------------------------------------------
		Function and Usage:
			-	Returns the data that is stored in EEPROM. The data includes
				the following elements
					- 	a version number to identify the EEPROM data
					- 	time zone ID
					-	standard time zone offset 	
					-	current time zone offset	
					-	Next DST transition time	
					-	Next DST transition offset

				Notes: 	
					-	Offsets are recorded as "+/- hours"
					-	Some offsets include fractional hours
				
		Syntax: 
			-	tzInfo_t <variable name>  = tzGetInfo(); 
			
		Return: A tzInfo_t structure containing the EEPROM data.
		
		Example:	
				The command: 	tzinfo_t eeprom = tzGetInfo();

				returns a tzInfo_t structure that exposes the following
				variable names:
					eeprom.version				<-- char[10]
					eeprom.zoneId				<-- char[65]
					eeprom.stdOffset			<--	float
					eeprom.currentOffset		<-- float
					eeprom.transitionTime		<--	time_t	
					eeprom.transitionOffset		<-- float	
					

	tzGetSetupReturnMsg()) ----------------------------------------------------

		Function and Usage:					
			-	When tzSetup runs, it records an exit message. This function
				returns that message. 
	
		Syntax: 
			-	tzGetSetupReturnMsg(); 
			
		Return: tzSetup Return message
	
	
	tzSetTransitionTime() -----------------------------------------------------
	
		Function and Usage:
			-	This method allows developers to perform DST transitions for
				testing purposes. 
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
			-	tzSetTransitionTime((time_t)<epoch seconds UCT>);
		
		Return:	none
		
		Examples: 
			-	To trigger an immediate transition ...
					tzSetTransitionTime(Time.now());
			-	to trigger a transition in 1 hour ...
					tzSetTransitionTime(Time.now() + (60 * 60));


	tzWipeEEPROM() ------------------------------------------------------------
	
		Function and Usage:
			-	This method allows developers to erase the time zone offset and 
				DST transition data from the EEPROM.
			-	If timezone data is not maintained at the default location (0)
				in EEPROM, the appropriate tzSetEepromLocation() statement must
				be executed before this method is invoked. This method does a 
				validation to help prevent erasure of the wrong data, but it's
				best not to tempt fate.
			-	Developers will find this method handy as they validate that
				default time zone ID settings work as expected.
			-	This method is FOR DEVELOPMENT AND TEST USAGE ONLY. tzLib will
				not work properly if it is left in your final code. For 
				example, tzLoop will not know when to perform DST transitions. 
				tzSetup will revert to the default time zone ID on each 
				reboot, and won't even have time zone offset information if the
				network is down. USE THIS METHOD WITH CAUTION !!!
		Syntax:
			-	tzWipeEEPROM();
		
		Return:	none
