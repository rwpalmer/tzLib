
#include "tzLib.h"

void displaytzInfo(bool = true);

    struct zones {
        char ch;
        char id[65];
    } zone[] = {{'a',"Africa/Johannesburg"},{'b',"Brazil/East"},{'c',"America/Chicago"},{'d',"Asia/Dhaka"},{'e',"Asia/Saigon"},{'f',"America/St_Johns"},{'i',"Asia/Tehran"},
                {'j',"Japan"},{'k',"America/Indiana/Knox"},{'m',"Pacific/Marquesas"},{'n',"Canada/Newfoundland"},{'p',"Europe/Amsterdam"},{'s',"Singapore"},
                {'t',"Australia/Tasmania"},{'v',"Asia/Tel_Aviv"},{'x',"Invalid"},{'z',"Pacific/Chatham"}};
    bool topMenu = true;


void setup() {

    //tzWipeEEPROM();   // <-- When active, this erases time zone data from the EEPROM, so testing can be done with a "new" device. 
    tzSetDefaultZoneId((char*)"America/New_York");  // <-- This changes the default time zone assigned to new devices from "UCT" to ...
    tzSetEepromLocation(256);   //  <-- This sets the EEPROM location where time zone information should be stored (default = 0);
    tzSetHttpHost((char*)"208.85.39.75");
    tzSetHttpPath((char*)"/tzLib/getJSON.php");
    

    tzSetup();  // <----------- This function configures the devices time zone offset, and DST settings

    Serial.begin(9600);
    for (int i = 10; i > 0; i--) {  //  <-- Give a serial console user time to connect.
        Serial.print("\rStarting in ");
        Serial.print(String(i));
        Serial.print(" Seconds ... ");
        delay(1000);
     }
    Serial.print("\r                                   \r");
    displaytzPrompt();       
}


void loop() {
    
    tzLoop();   // <----------- This tzLib method automatically performs transitons to and from daylight savings time. 

    // The following code demonstrates how timezone information can be displayed and how the timezones can be changed.
   
    if (Serial.available()) {
        char ch = Serial.read();
        Serial.print('\b');
        Serial.println(ch);
        if (topMenu) {
            if (ch == 'q')  {   //  <------------------------ Allows user to display current EEProm settings
                Serial.println("-------------------------------------------------- QUERYING EEPROM SETTINGS ---");
                displaytzInfo();
                displaytzPrompt();
            } else if (ch == 't') {  //  <------------------------ Trigger a pending DST transition to occur NOW
                tzInfo_t tzInfo = tzGetInfo();
                if (tzInfo.transitionTime == 0) {
                    Serial.println("Sorry, No DST transition is pending for this time zone.");
                    Serial.println();
                } else {
                    Serial.println("----------------------------------------------- SETTINGS BEFORE DST TRANSITION ---");
                    displaytzInfo(false);
                    tzSetTransitionTime(Time.now()); // <-- This method call triggers the transistion
                    tzLoop();
                    Serial.println("------------------------------------------------ SETTINGS AFTER DST TRANSITION ---");
                    displaytzInfo();
                    displaytzPrompt();
                }
            } else if (ch == 'c') {
                Serial.println("---------------------------------------------------------- TIME ZONE SELECTION ---");
                for (int i=0; i<(sizeof(zone)/sizeof(zone[1])); i++) {
                    Serial.print("   ");
                    Serial.print(zone[i].ch);
                    Serial.print(" = ");
                    Serial.println(zone[i].id);
                }
                    Serial.print("\n   You may select one of the above timezones, or press esc to return:  ");
                    topMenu = false;
            }
        } else {
            if (ch == 27) { //  <-- handle the esc key
                topMenu = true;
                Serial.println("<<<< Exiting Time Zone Selection >>>");
                displaytzPrompt();
            }
            for (int i=0; i<(sizeof(zone)/sizeof(zone[1])); i++) {
                if (ch == zone[i].ch) {
                    tzSetup(zone[i].id);
                    Serial.println("------------------------------------------------------------- TIME ZONE CHANGED ---");
                    topMenu = true;
                    displaytzInfo();
                    displaytzPrompt();
                    break;
                }
            }
        }
    }
}


void displaytzInfo(bool msg) {
    tzInfo_t tzInfo = tzGetInfo();  //  <-- This function gets the time zone offset and DST transition data from EEPROM
    Serial.println(String(tzInfo.zoneId));
    Serial.print("         Current Offset: ");
    Serial.print(String(tzInfo.currentOffset));
    Serial.print(" hours, Standard Offset: ");
    Serial.print(String(tzInfo.stdOffset));
    Serial.println(" hours");
    if (tzInfo.transitionTime == 0) {
        Serial.println("        No DST transitions pending.");
    } else {
    Serial.print("     Next DST Transiton:  ");
    Serial.print(String(tzInfo.transitionTime));
    Serial.print(" (epoch seconds UCT) = ");
    Serial.print(Time.format(tzInfo.transitionTime, TIME_FORMAT_DEFAULT));
    Serial.println(" (Local Time)");
    Serial.print("      Transition Offset: ");
    Serial.print(String(tzInfo.transitionOffset));
    Serial.println(" hours");
    }
       if (msg) {
        Serial.print("            ");
        Serial.println(tzGetSetupReturnMsg());   //  <-- This function returns a status message from tzSetup()
    }
    Serial.println("Device Settings: - - - - - - - - - - - - - - - - - -");
    Serial.print("            Time.zone() = ");
    Serial.print(String(Time.zone()));
    Serial.print("           Time.isDST() = ");
    Serial.println(String(Time.isDST()));
    Serial.print("    Time.getDSTOffset() = ");
    Serial.print(String(Time.getDSTOffset()));
    Serial.println(" hours");
    Serial.print("    The current time in this time zone is: " );
    Serial.print(Time.format(Time.now(), TIME_FORMAT_DEFAULT));
    Serial.print("   ");
    Serial.println(tzGetAbbr());
}


void displaytzPrompt() {
    Serial.println("-----------------------------------------------------------------------------------");
    Serial.print("Press 'q' to query EEPROM, 'c' to change time zone, or 't' to transition now:  ");
}

























