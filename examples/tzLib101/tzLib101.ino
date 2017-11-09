#include "tzLib.h"

/* Module: tzLib101.ino
This firmware example demonstrates all of the methods that most tzLib users will need. 

	WARNING: 	tzLib requires 128 bytes of EEPROM storage. In tzLib parlance, this 128 byte block is called the tzBlock. 
					- Default location: 	The first 128 bytes of EEPROM storage.
					- Alternate location:	Use tzLib.setEepromStartByte() to select any location
											from 0 to (EEPROM.length() - 128);
					- Do NOT use tzLib unless the required EEPROM storage is available. 

	This example demonstrates: 
		The three required tzLib methods
			tzLib.begin()                   Initializes the library (required, must run before any other tzLib methods)
			tzLib.setLocalTime()            Sets the device's local time settings, runs in setup()
			tzLib.maintainLocalTime()       Keeps time zone information current, and performs DST transitions (runs in loop())
		
		Two "often used" configuration methods. If used, they should be positioned before tzLib.setLocalTime() in setup();
		tzLib.setDefaultZone()          Overrides tzLib's default time zone, which is "UTC". (This method is rarely omitted)
		tzLib.setEepromStartByte()		Overrides tzLib's default location for the tzBlock in EEPROM, which is byte 0.
										(Not needed unless other data is stored in the first 128 bytes of EEPROM)

		Three methods for developers to obtain information from tzLib. 
			tzLib.getHttpStatus()           Reports the status of HTTP communications
			tzLib.getZone()                 Returns the current time zone ID
			tzLib.getZoneAbbr()             Returns the current time zone Abbreviation (often changes with DST)
            
    Running the example requires a serial console. The firmware has a 10 second countdown timer to give users
    time to connect their console. 
	
    A sample of the example's output is included in the comments, at the bottom of this file. 

*/
// ----------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------- setup()
void setup() {
    
    // Enable communication with the serial console, and give the user 10 seconds to connect.
    Serial.begin(9600);
    for (int i = 10; i > 0; i--) {  //  <-- Give a serial console user time to connect.
        Serial.print("\rStarting in ");
        Serial.print(String(i));
        Serial.print(" Seconds ... ");
        delay(1000);
     }
    Serial.print("\r                                   \r");

    //  Initialize tzLib 
    tzLib.begin();      // <-- Required, MUST be run before any other tzLib method is run ...
    
    //  Set the default time zone
    tzLib.setDefaultZone("America/New_York");   // <-- If not specified, "UTC" will be the default
	
	/*  Select the EEPROM storage location where tzLib should store the 128-byte block of time zone data that it needs.
		   FYI, In tzLib parlance, the 128-byte block of time zone data is referred to as the "tzBlock"	.
				The tzBlock stores:	- The time zone ID required to query the HTTP server for current time zone data
									- The information required set local time properly each time the device
									  reboots ... whether or not network connectivity exists at that time.
									- The information required to perform DST transitions when they are scheduled.
				If a tzBlock already exists at another location in EEPROM, it will be moved to the selected location,
				and the original location will be overwritten with '0xFF' characters. */
	tzLib.setEepromStartByte(512); // <-- Required if default location (0) is not available
    
    //  Set the local time
    tzLib.setLocalTime();   /* <-- Required, Will set local time based on:
                                        1. A time zone ID stored in EEPROM
                                        2. The default time zone if no ID is found in EEPROM
										3. The time zone "UTC", if no ID in EEPROM and no user specified default.
                            */

        //  OPTIONAL CALLS TO tzLib and Particle  METHODS ----------------------------------
            Serial.println("\n\n\ttzLib101 -------------------------------------------------------------------------");
        //  tzLib communicates with an HTTP server to obtain time zone details. tzLib.getHttpStatus() 
        //  displays the status of that communication after tzLib.setLocalTime() is run.
            Serial.print("\n\ttzLib.getHTTPStatus() returns:\t\t");
            Serial.println(tzLib.getHttpStatus());
            
        //  tzLib exposes the local time zone ID:
            Serial.print("\ttzLib.getZone() returns:\t\t");
            Serial.println(tzLib.getZone());

        //  tzLib exposes the local time zone's abbreviation. (This varies with DST)
            Serial.print("\ttzLib.getZoneAbbr() returns:\t\t");
            Serial.println(tzLib.getZoneAbbr());
            
        //  tzLib updates Particle device settings related to local time
            Serial.print("\tParticle Time.zone() returns:\t\t");
            Serial.println(Time.zone());
            
            Serial.println(Time.isDST()? "\tParticle isDst() returns:\t\ttrue":"\tParticle isDst() returns:\t\tfalse");

            Serial.print("\tParticle Time.getDSTOffset() returns:\t");
            Serial.println(Time.getDSTOffset());
            
        //  print local time
            
            Serial.print("\n\t\t\tLocal Time is:\t");
            Serial.println(Time.format(Time.now(), TIME_FORMAT_DEFAULT));

}
// ---------------------------------------------------------------------------------- loop()
void loop() {
    
    // Keep local time zone information stored in EEPROM current, and perform DST transitions
    tzLib.maintainLocalTime();	// <-- Required
}

/* 
----------------------------------------  S A M P L E  -  O U T P U T  -----------------------------------------

        tzLib101 -------------------------------------------------------------------------

        tzLib.getHTTPStatus() returns:          HTTP Processing Completed Normally
        tzLib.getZone() returns:                America/Los_Angeles
        tzLib.getZoneAbbr() returns:            PST
        Particle Time.zone() returns:           -8.00
        Particle isDst() returns:               false
        Particle Time.getDSTOffset() returns:   0.00

                        Local Time is:  Sun Nov  5 10:46:38 2017

----------------------------------------------------------------------------------------------------------------


--------------------------------------  T R O U B L E S H O O T I N G  -----------------------------------------

	If tzLib.getHttpStatus() did not return  "HTTP Processing Completed Normally", please review and resolve  the
	error message that was returned before consulting the information listed in this section.

	------------------------------------------------------------------------------------------------------------

        Problem:    You want tzLib.setLocalTime() to configure time for the default time zone but ...
                    it is configuring time for a time zone ID that is stored in EEPROM. 
                    
        Solution:   Erase the time zone ID stored in EEPROM by inserting the following command before the call
                    to tzLib.setLocalTime():
                    
                                tzLib.eraseTzEeprom();
                        
                    IMPORTANT NOTE: 
                        The above command is FOR TESTING ONLY. It allows users to test the "out-of-box", or
                        "new-device" scenario. If the command is left in production code, tzLib will not
                        remember ANY time zone details when the device is rebooted.
                        
                    FYI:
                        tzLib.eraseTzEeprom() does not erase the device's entire EEPROM. The method only erases
                        a 128-byte block of EEPROM that contains time zone information. We refer to this block
                        as the "tzBlock". By default, the tzBlock is located at EEPROM byte 0, but developers
                        can select a different location.  
                        
                    
*/



