#include "tzLib.h"

/*  Module tzLib301

    This firmware demonstrates how tzLib users can change their device's time zone via the web. 
	
    Running the example requires a serial console. The firmware has a 10 second countdown timer to
    give users time to connect their console. 
	
	In the comments at the bottom of this file, you will find two things:
		1. 	Sample output generated from this firmware
		2. 	Some HTML, with instrutions, that you can use to use to change your device's time zone
			via your computer. 
	
*/

// ----------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------- setup()
void setup() {
	
	// Register the Particle Function that will allow time zone changes via the web --------------- < NOTE THIS
    Particle.function("webChangeTz", webChangeTz);    
   
    // Enable communication with the serial console, and give the user 10 seconds to connect.
    Serial.begin(9600);
    for (int i = 10; i > 0; i--) {  //  <-- Give a serial console user time to connect.
        Serial.print("\rStarting in ");
        Serial.print(String(i));
        Serial.print(" Seconds ... ");
        delay(1000);
     }
    Serial.print("\r                                   \r");

    // Use tzLib to set the devices local time
    tzLib.begin(); 
    tzLib.setDefaultZone("America/New_York");   
    tzLib.setLocalTime();  

    // Display local time settings
    Serial.println("\n\n\ttzLib301 -------------------------------------------------------------------------");
    Serial.print("\n\ttzLib.getHTTPStatus() returns:\t\t");
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
    Serial.println("\n\n\t\t<<< WAITING FOR A TIME ZONE CHANGE ... FROM THE WEB >>>");
}


// ----------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------- loop()
void loop() {
    
    // Keep local time zone information stored in EEPROM current, and perform DST transitions
    tzLib.maintainLocalTime();
}


//--------------------------------------------------------------------------------------------
//---------------  PARTICLE FUNCTION TO ENABLE WEB BASED TIME ZONE CHANGES  ------------------
//--------------------------------------------------------------------------------------------

int webChangeTz(String id) {
    char zoneId[65];
    id.toCharArray(zoneId, sizeof(zoneId));
    tzLib.changeZone(zoneId);
    
    // Display data for new time zone
	Serial.println("\n\n\tReceived Time Zone Change  -------------------------------------------------------");
	Serial.print("\n\ttzLib.getHTTPStatus() returns:\t\t");
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

/* 
----------------------------------------  S A M P L E  -  O U T P U T  -----------------------------------------


        tzLib301 -------------------------------------------------------------------------

        tzLib.getHTTPStatus() returns:          HTTP Processing Completed Normally
        tzLib.getZone() returns:                America/Los_Angeles
        tzLib.getZoneAbbr() returns:            PST
        Particle Time.zone() returns:           -8.00
        Particle isDst() returns:               false
        Particle Time.getDSTOffset() returns:   0.00

                        Local Time is:  Mon Nov  6 11:32:04 2017


                <<< WAITING FOR A TIME ZONE CHANGE ... FROM THE WEB >>>


        Received Time Zone Change  -------------------------------------------------------

        tzLib.getHTTPStatus() returns:          HTTP Processing Completed Normally
        tzLib.getZone() returns:                Pacific/Honolulu
        tzLib.getZoneAbbr() returns:            HST
        Particle Time.zone() returns:           -10.00
        Particle isDst() returns:               false
        Particle Time.getDSTOffset() returns:   0.00

                        Local Time is:  Mon Nov  6 09:32:32 2017


----------------------------------------------------------------------------------------------------------------
*/



/* The following HTML can be used to change timezone via the web.

    1. 	Copy the HTML and save it to a file named tzLib301.html
	
    2. 	Edit the file:
			Replace "<_YOUR_DEVICE_ID_>" with your device's ID.
			Replace "<_YOUR_ACCESS_TOKEN_>" with your Particle access token.
	
    3. 	Open the file in your browser, select a time zone, and click the "Change Timezone" button.

	4. 	Assuming your device is attached to a serial console, you should see output like the sample output displayed above. 
	
 Note: 	If you prefer to test time zones that you are more familiar with, just edit the HTML.
		Here is a link to a good source for valid time zone IDs:
			https://en.wikipedia.org/wiki/List_of_tz_database_time_zones


<!DOCTYPE>
<html>
  <body>
  <form action="https://api.particle.io/v1/devices/<_YOUR_DEVICE_ID_>/webChangeTz?access_token=<_YOUR_ACCESS_TOKEN_>" method="POST">
    Please select a timezone ... <br>
    <input type="radio" name="args" value="America/Chicago">Chicago<br>
    <input type="radio" name="args" value="America/Denver">Denver<br>
    <input type="radio" name="args" value="Pacific/Honolulu">Hololulu<br><br>
    <input type="submit" value="Change Timezone">
  </form>
  </body>
</html>

*/