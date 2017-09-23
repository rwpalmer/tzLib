
#include <HttpClient.h>
#include "tzLib.h"


void setup() {
    
    tzWipeEEPROM(); //  <-- this method clears the tzLib settings in EEPROM ... making it look like this is the first time tzLib has been run. 
    tzSetDefaultZoneId((char*)"America/Chicago");   // <-- this method sets the time zone ID when no time zone setting ID is found in EEPROM.
    tzSetHttpHost("192.168.86.28"); // <-- this method sets the DNS Name or IP address of the HTTP server hosting time zone data.
    tzSetHttpPath("/tz/tzOffsetJSON/");    // <-- this method defines the path to time zone data on the HTTP server

    tzSetup();  /* <----------- This method:
                        1. Reads the user-selected timezone ID from EEPROM.
                        2. Contacts an HTTP server to obtain current time zone settings and DST
                           transition information. (This data is then saved to EEPROM so is available
                           should the system reboot when there is a network outage.) 
                        3. Sets the devices current time zone settings.
                */  
                
    Serial.begin(9600);
    for (int i = 12; i > 0; i--) {  //  <-- Wait for a serial console user to connect.
        Serial.print("\rStarting in ");
        Serial.print(String(i));
        Serial.print(" Seconds ... ");
        delay(1000);
    }
    Serial.println("\r                                   \r");   
    
    displayInfo();  // <-- This calls the function that displays information          
}



void loop() {
    
    tzLoop();   /* <----------- This function:
                        1. Automatically switches local time settings to reflect transition to or 
                           from DST at the appropriate time.
                        2. Periodically validates and refreshes the local time zone data in EEPROM.
                */  
}



void displayInfo() {

    tzInfo_t tzInfo = tzGetInfo();  //  <-- This function gets the time zone offset data from EEPROM
    Serial.print("TimeZone = ");
    Serial.println(String(tzInfo.zoneId));
    Serial.println("\nData Retrieved from HTTP Server:");
    Serial.print("        Standard Offset:  ");
    Serial.print(String(tzInfo.stdOffset));
    Serial.println(" hours");
    Serial.print("         Current Offset:  ");
    Serial.print(String(tzInfo.currentOffset));
    Serial.println(" hours");
    Serial.print("     Next DST Transiton:  ");
    Serial.print(String(tzInfo.transitionTime));
    Serial.print(" (epoch seconds UCT) = ");
    Serial.print(Time.format(tzInfo.transitionTime, TIME_FORMAT_DEFAULT));
    Serial.println(" (Local Time)");
    Serial.print("      Transition Offset:  ");
    Serial.print(String(tzInfo.transitionOffset));
    Serial.println(" hours");
    Serial.println("\n Device Settings:");
    Serial.print("            Time.zone() = ");
    Serial.println(String(Time.zone()));
    Serial.print("    Time.getDSTOffset() = ");
    Serial.print(String(Time.getDSTOffset()));
    Serial.println(" hours");
    Serial.print("           Time.isDST() = ");
    Serial.println(String(Time.isDST()));
    Serial.print("\nThe current time in this time zone is: " );
    Serial.println(Time.format(Time.now(), TIME_FORMAT_DEFAULT));

}

/* --------------------------------------------------------------------------------------------

                SAMPLE OUTPUT GENERATED TO THE SERIAL CONSOLE BY THIS FIRMWARE

TimeZone = America/Chicago

Data Retrieved from HTTP Server:
        Standard Offset:  -6.000000 hours
         Current Offset:  -5.000000 hours
     Next DST Transiton:  1509865200 (epoch seconds UCT) = Sun Nov  5 02:00:00 2017 (Local Time)
      Transition Offset:  -6.000000 hours

 Device Settings:
            Time.zone() = -6.000000
    Time.getDSTOffset() = 1.000000 hours
           Time.isDST() = 1

The current time in this time zone is: Wed Sep 20 20:12:23 2017

-------------------------------------------------------------------------------------------- */
























