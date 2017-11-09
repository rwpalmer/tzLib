#include "tzLib.h"

/*  Module: tzLib201.ino

This firmware demonstrates how to configure tzLib to use an alternate HTTP server, 
and how to test DST transitions.

    Three methods allow developers to use an alternate, HTTP server. 
                tzLib.setHostName()
                tzLib.setHostPort()
                tzLib.setHostPath()
			FYI: Instructions for how to create a virtual HTTP server for tzLib are documented @ 					    https://github.com/rwpalmer/tzLib/blob/master/doc/tzLib_%20ServerBuild.md

    DST transition testing is enabled with two methods
                tzLib.setNextTransitionTime()
                tzLib.transitionNow()

   
    Running the example requires a serial console. The firmware has a 10 second countdown timer to give users
    time to connect their console. 
	
	A sample of the example's output is included in the comments, at the bottom of this file. 
*/

static time_t tTime = 0;
static bool transition = true;
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
    tzLib.begin();      // <-- Required, Must be run before any other tzLib method is run ...
    
    //  Set the default time zone
    tzLib.setDefaultZone("America/New_York");   // <-- If not run, "UTC" will be the default
 
 
    
    //--------------------------------------------------------------------------------------------
    //---------- H T T P    S E R V E R    C O N F I G U R A T I O N    M E T H O D S ------------
    //--------------------------------------------------------------------------------------------
    // These configuration methods should be executed before tzLib.setLocalTime() is run.
    
    // Set the alternate HTTP server's name, port, and path
    tzLib.setHostName("208.85.39.75");          //  <-- string can be server's DNS host name or IP address
    tzLib.setHostPort(80);                      //  <-- servers HTTP port number
    tzLib.setHostPath("/tzLib/getJSON.php");    //  <-- server path to getJSON.php (must start with '/')
    
    //--------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------

	// Configure the device's local time settings
    tzLib.setLocalTime(); 

    Serial.println("\n\n\ttzLib201 -------------------------------------------------------------------------");
    Serial.print("\n\ttzLib.getHttpStatus() returns:\t\t");
    Serial.println(tzLib.getHttpStatus());
    Serial.print("\ttzLib.getZone() returns:\t\t");
    Serial.println(tzLib.getZone());
    Serial.print("\ttzLib.getZoneAbbr() returns:\t\t");
    Serial.println(tzLib.getZoneAbbr());
    Serial.print("\tParticle Time.zone() returns:\t\t");
    Serial.println(Time.zone());
    Serial.println(Time.isDST()? "\tParticle isDst() returns:\t\ttrue":"\tParticle isDst() returns:\t\tfalse");
    Serial.print("\tParticle Time.getDSTOffset() returns:\t");
    Serial.println(Time.getDSTOffset());
    Serial.print("\n\t\t\tLocal Time is:\t");
    Serial.println(Time.format(Time.now(), TIME_FORMAT_DEFAULT));
 
}
// ---------------------------------------------------------------------------------- loop()
void loop() {
    
    // Keep local time zone information stored in EEPROM current, and perform DST transitions
    tzLib.maintainLocalTime();


/*  --------------------------------------------------------------------------------------------
    --------------------- T R A N S I T I O N    T E S T    M E T H O D S ----------------------
    --------------------------------------------------------------------------------------------
    
    For those who wish to peform transition testing, tzLib provides two methods that will  alter
    the current time zone's next transition time:
      
      1. tzLib.setNextTransitionTime(time_t) sets the transtition time to time_t.
	  2. tzLib.transitionNow() is short hand for tzLib.setNextTransitonTime(Time.now())
      
    As it does in the production environment, tzLib.maintainLocalTime() will perform the 
    transition when Time.now() is not less than the transition time.

    It should be noted that these test methods do nothing unless the current time zone observes
    DST and there is a pending DST transition. When a transition is triggered, the pending
    transition is cleared until the next time tzLib.setLocalTime() is run. This normally occurs
    on reboots and scheduled refreshes, but tzLib.setLocalTime() can be called (at will) in a
    test program if a reset is desired. 
    
    tzLib.setNextTransitionTime(time_t) is demonstrated below. Since we are operating in loop(),
	the "if (transition)" and "if (!tTime)" lines are included to make sure the sample code is
	not executed over and over again.

*/
    
    if (transition) {
        Serial.println("\n\n\t\t<<< TRANSITION SCHEDULED IN 5 SECONDS ... PLEASE WAIT >>>");
        tTime = Time.now() + 5;
        tzLib.setNextTransitionTime(tTime);    // <-- Note: time is specified in seconds
        transition = false;
    }

    if ( ! tTime == 0) {
        if (Time.now() > (tTime + 1)) {
            tTime = 0; 
            // same OPTIONAL CALLS as above ... without comments
            // HTTP status is not displayed because transitions do not perform HTTP activity
            Serial.println("\n\n\ttzLib201 -------------------------------------------------------------------------");
            Serial.print("\ttzLib.getZone() returns:\t\t");
            Serial.println(tzLib.getZone());
            Serial.print("\ttzLib.getZoneAbbr() returns:\t\t");
            Serial.println(tzLib.getZoneAbbr());
            Serial.print("\tParticle Time.zone() returns:\t\t");
            Serial.println(Time.zone());
            Serial.println(Time.isDST()? "\tParticle isDst() returns:\t\ttrue":"\tParticle isDst() returns:\t\tfalse");
            Serial.print("\tParticle Time.getDSTOffset() returns:\t");
            Serial.println(Time.getDSTOffset());
            Serial.print("\n\t\t\tLocal Time is:\t");
            Serial.println(Time.format(Time.now(), TIME_FORMAT_DEFAULT));
        }
    }
}

/* 


----------------------------------------  S A M P L E  -  O U T P U T  -----------------------------------------


        tzLib201 -------------------------------------------------------------------------

        tzLib.getHttpStatus() returns:          HTTP Processing Completed Normally
        tzLib.getZone() returns:                America/New_York
        tzLib.getZoneAbbr() returns:            EST
        Particle Time.zone() returns:           -5.00
        Particle isDst() returns:               false
        Particle Time.getDSTOffset() returns:   0.00

                        Local Time is:  Mon Nov  6 12:26:10 2017


                <<< TRANSITION SCHEDULED IN 5 SECONDS ... PLEASE WAIT >>>


        tzLib201 -------------------------------------------------------------------------
        tzLib.getZone() returns:                America/New_York
        tzLib.getZoneAbbr() returns:            EDT
        Particle Time.zone() returns:           -5.00
        Particle isDst() returns:               true
        Particle Time.getDSTOffset() returns:   1.00

                        Local Time is:  Mon Nov  6 13:26:17 2017




----------------------------------------------------------------------------------------------------------------


--------------------------------------  T R O U B L E S H O O T I N G  -----------------------------------------

	If tzLib.getHttpStatus() did not return  "HTTP Processing Completed Normally", please review and resolve  the
	error message that was returned before consulting the information listed in this section.

	------------------------------------------------------------------------------------------------------------

		Problem 1:		No DST transition takes place after the 5 second delay. This will occur if the configured
						time zone does not observe DST.
					
		Solution 1:		If the default time zone ID was changed from "America/New_York" to a time zone that does
						not observe DST, the default time zone should be changed to a time zone that does observe
						DST.
						
						Whether or not the above change was made, follow solution 2 below to erase the EEPROM, so
						the transition will apply to the default time zone that is configured in this firmware.

	------------------------------------------------------------------------------------------------------------
	
        Problem 2:    	You want tzLib.setLocalTime() to configure time for the default time zone but ...
						it is configuring time for a time zone ID that is stored in EEPROM. 
					
                    
        Solution 2: 	Erase the time zone ID stored in EEPROM by inserting the following command before the call
						to tzLib.setLocalTime():
                    
                                tzLib.eraseTzEeprom();
                        
						IMPORTANT NOTE: 
							The above command is FOR TESTING ONLY. It allows users to test the "out-of-box", or
							"new-device" scenario. If the command is left in production code, tzLib will not
							remember ANY time zone details when the device is rebooted.
                        
                    FYI:
                        tzLib.eraseTzEeprom() does not erase the device's entire EEPROM. The method only erases
                        the tzBlock that contains time zone information. Erasing involves writing
						'0xFF' characters to the EEPROM memory location where the tzBlock was stored. 
 
*/

