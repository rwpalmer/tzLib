#include "tzLib.h"

/*      Library: tzLib
        Module: TzLib.cpp contains tzLib's  core elements (data and methods). 
*/

// ---------------------------------------------------------------------------- begin
// TzLib's constructor ... MUST BE THE FIRST TzLIb command that is executed on the device
void TzLib::begin(void) {
        // initialize default values
        strncpy(defaultZone, "UTC", sizeof(defaultZone));
        strncpy(hostName, "208.85.39.75", sizeof(hostName));
        strncpy(hostPath, "/tzLib/getJSON.php", sizeof(hostPath));
        hostPort = 80;
        eepromStartByte = searchForTzEeprom();
        if (eepromStartByte > -1) {                   //  If tzEeprom was found in EEPROM ...
            EEPROM.get(eepromStartByte,tzEeprom);     //      copy tzEeprom to static memory
            tzEepromExists = true;
        } else {
            tzEepromExists = false;
        }
        time_t tzRefreshTime = Time.now() + ( 3 * tzBlockRefreshInterval);
        #ifdef LOGGING
            Serial.begin();
            Serial.println("-------------------------------------------------- TzLib::begin()");
            Serial.print("tzLib>\tEepromStartByte = ");
            Serial.println(eepromStartByte);
            tzEeprom.log("tzEeprom");
            Serial.print("tzLib>\tSizeof(tzEeprom) = ");
            Serial.println(sizeof(tzEeprom));
        #endif

}

// ---------------------------------------------------------------------------- setLocalTime()
// Select the device's time zone configure local time settings
int TzLib::setLocalTime(void) {
	if (tzEepromExists && (strlen(tzEeprom.id) > 0)) { 	// if a time zone ID is stored in EEPROM, we use it ...
		return(setLocalTime(tzEeprom.id));
	} else {											// otherwise, we use the default time zone ID
		return(setLocalTime(defaultZone));
	}
}	


// ---------------------------------------------------------------------------- maintainLocalTime()
void TzLib::maintainLocalTime() {
    
    // Perform a DST transition when the scheduled transition time arrives
    if ((tzEeprom.tranTime > 0) && !(tzEeprom.tranTime > Time.now())) {
        #ifdef LOGGING
            Serial.println("----- TzLib::maintainLocalTime()");
            Serial.print("Triggering the Transition @ ");
            Serial.println(Time.format(Time.now()));
            Serial.print("tzEeprom.tranTime = ");
            Serial.println(tzEeprom.tranTime);
        #endif
        transitionNow();
        tzEeprom.tranTime = 0; // precaution to prevent retriggering in an error condition
    }
    // Verify or UPdate the TzBlock in EEPROM when the scheduled refresh time arrives
    if (Time.now() >= eepromRefreshTime) {
        setLocalTime();
    }
}

// ---------------------------------------------------------------------------- setDefaultZone()
// Sets the default time zone
void TzLib::setDefaultZone(char* id) {
    strcpy(defaultZone, id);
}

// ---------------------------------------------------------------------------- getZone()
// Gets the time zone ID
char* TzLib::getZone(void) {
    return (char*)tzEeprom.id;
}

// ---------------------------------------------------------------------------- getZoneAbbr()
// Gets the current time zone abbreviation which often changes with DST transitions
char* TzLib::getZoneAbbr(void) {
    return (char*)tzEeprom.curAbbr;
}

// ---------------------------------------------------------------------------- changeZone()
// Changes the time zone ID and updates the devices local time settings.
void TzLib::changeZone(char* id) {
	setLocalTime(id);
}

// ---------------------------------------------------------------------------- setEepromStartByte()
// Defines where the TzBlock will be stored in EEPROM
void TzLib::setEepromStartByte(int sb) {
    if (sb == eepromStartByte) {
        return;
    }
    if ((sb > 0) && !(sb > (EEPROM.length() - sizeof(tzEeprom)))){
        #ifdef LOGGING
            Serial.println("\n\r----- setEepromStartByte()");
        #endif
        if (tzEepromExists) {
            #ifdef LOGGING
                Serial.print("tzLib>\tWriting TzBlock to EEPROM byte ");
                Serial.println(sb);
            #endif
            eraseTzEeprom();
            EEPROM.put(sb,tzEeprom);
 			tzEepromExists = true;
       } else {
            #ifdef LOGGING
                Serial.print ("tzLib>\teepromStartByte has been set to ");
                Serial.println(sb);
            #endif
        }
        eepromStartByte = sb;
    } else {
        #ifdef LOGGING
            Serial.println("tzLib>\tERROR: Attempt to set EepromStartByte rejected: byte# not in range");
        #endif
    }
    return;
}

// ---------------------------------------------------------------------------- setHostName()
// Defines the DNS name or IP address of the HTTP server
void TzLib::setHostName(char* host) {
	strncpy(hostName, host, sizeof(hostName));
}

// ---------------------------------------------------------------------------- setHostPath()
// Defines the path to the getJson.php file on the HTTP server
void TzLib::setHostPath(char* path) {
	strncpy(hostPath, path, sizeof(hostPath));
}

// ---------------------------------------------------------------------------- setHostPort()
// Defines the port that will be used for HTTP communications
void TzLib::setHostPort(uint16_t port) {
    hostPort = port;
}

// ---------------------------------------------------------------------------- transitionNow()
// Allows tzLib users to instantly simulate a transition for testing purposes
void TzLib::transitionNow(void) {
    if (tzEeprom.tranTime > 0) {
        tzEeprom.tranTime = 0;
        tzEeprom.curOffset = tzEeprom.tranOffset;
        tzEeprom.tranOffset = 0;
        strcpy(tzEeprom.curAbbr, tzEeprom.tranAbbr);
        tzEeprom.tranAbbr[0] = '\0';
        EEPROM.put(eepromStartByte, tzEeprom);
        updateDeviceSettings();
        #ifdef LOGGING
            Serial.println("----- TzLib::transitionNow()");
            Serial.println(Time.format(Time.now()));
            Serial.print("EepromStartByte = ");
            Serial.println(eepromStartByte);
            tzEeprom.log("tzEeprom");
        #endif
    }
    return;
}

// ---------------------------------------------------------------------------- setNextTransitionTime()
// Allows tzLib users to simulate a transition at a future time for testing purposes
// For example: "tzLib.setNextTransitionTime(Time.now() + 30);", for 30 seconds from now
int TzLib::setNextTransitionTime(time_t time) {
    tzEeprom.tranTime = time;
    #ifdef LOGGING
        Serial.println("----- TzLib::setNextTransitionTime()");
        Serial.print("NextTransitionTime = ");
        Serial.println(Time.format(Time.now()));
    #endif
}
    
// ---------------------------------------------------------------------------- eraseTzEeprom()
// Erases the TzBlock from EEPROM memory
// Allows tzLib users to simulate how tzLib will perform on a new device. 
void TzLib::eraseTzEeprom(void) {
    TzBlock tzBlk;
    char signature[sizeof(TZ_SIGNATURE)];
    EEPROM.get(eepromStartByte, signature);
    if (strcmp(signature, TZ_SIGNATURE) == 0) {
        memset(&tzBlk, 0xFF, sizeof(tzBlk));
        EEPROM.put(eepromStartByte, tzBlk);
        #ifdef LOGGING
            Serial.print("tzLib>\tErased TzBlock @ EEPROM location ");
            Serial.println(eepromStartByte);
        #endif
        tzEepromExists = false;
    	strcpy(tzEeprom.id,"");
    	tzEeprom.stdOffset = 0;
    	tzEeprom.curOffset = 0;
    	strcpy(tzEeprom.curAbbr,"");
    	tzEeprom.tranTime = 0;
    	tzEeprom.tranOffset = 0;
    	strcpy(tzEeprom.tranAbbr,"");
    }
}

// ---------------------------------------------------------------------------- getHttpStatus()
char* TzLib::getHttpStatus(void) {
    return (char*)statusMsg;
}



// ______________________________________________________________________________________________

//                    P R I V A T E    M E T H O D S    B E L O W


// ---------------------------------------------------------------------------- setLocalTime(char* id)
int TzLib::setLocalTime(char* tzID) {
   // Prepare to query server for timezone information ... 
    TzBlock tzWeb;
    strncpy(tzWeb.id, tzID, sizeof(tzWeb.id));
        #ifdef LOGGING
            Serial.println("\n\r-------------------------------------------------- TzLib::setLocalTime()");
            Serial.print("tzLib>\teepromStartByte =  ");
            Serial.println(eepromStartByte);
            Serial.println(tzEepromExists? "tzLib>\tzEepromExists = true":"tzLib>\tzEepromExists = false");
            Serial.print("tzLib>\tdefaultZone = ");
            Serial.println(defaultZone);
            Serial.print("tzLib>\ttzEeprom.id = ");
            Serial.println(tzEeprom.id);
            Serial.print("tzLib>\tReady to Query: tzWeb = ");
            Serial.println(tzWeb.id);
        #endif
            
    // Query the HTTP Server
    Http http;
    Json json;
    char jsonStr[129];
    int exitStatus = EXIT_FAILURE;
    json.encodeQuery(jsonStr, tzWeb.id);
    statusMsg[0] = '\0';
    int statusCode = http.getJson(hostName, hostPort, hostPath, jsonStr, sizeof(jsonStr), statusMsg, sizeof(statusMsg));
    // note: jsonStr provides http.post() with the query string, and
    //       http.post() overwrites that with the JSON returned from
    //       the HTTP server.
    if (statusCode == 200) {
        // Update the 'tzWeb' TzBlock with the JSON returned from the HTTP server
        if (tzWeb.applyJson(jsonStr) == EXIT_SUCCESS) {

        		#ifdef LOGGING
        		    tzWeb.log("tzWeb");
        		#endif
				
            //if the 'tzWeb' TzBlock has new data, update EEPROM
            if ( !tzEeprom.equals(tzWeb)) {
				if (eepromStartByte == -1) { // <-- Occurs if no TzBlock was found in EEPROM, AND no startByte has been designated.
					eepromStartByte = 0;
				}
                EEPROM.put(eepromStartByte,tzWeb);
                tzEepromExists = true;
                EEPROM.get(eepromStartByte,tzEeprom);
            } else {  
                #ifdef LOGGING
                    Serial.println("tzLib>\tSkipping TzBlock update");
                #endif
            }
        }
        exitStatus = EXIT_SUCCESS;
    }
    // update the devices local time settings based on EEPROM alone
    updateDeviceSettings();
    int refresh_multiplier = 1;
    
    // If the tz has no DST scheduled transitions, we will trebble the refresh Interval
    if ((tzWeb.tranTime == 0) && (tzWeb.stdOffset == tzWeb.curOffset)) {
        refresh_multiplier = 3;
    }
    eepromRefreshTime = Time.now() + (tzBlockRefreshInterval * refresh_multiplier);
    return exitStatus;
}

// ---------------------------------------------------------------------------- searchForTzBlock()
// Locates the TzBlock stored in EEPROM. 
//      Returns -1 when  TzBlock is NOT found
//      Returns the location of TzBlocks first byte when a TzBlock is found
int TzLib::searchForTzEeprom(void) {
    // The usable EEPROM location ranges from zero to (EEPROM storage size - tzBlockSize).
    int endUsableRange = (EEPROM.length() - sizeof(tzEeprom));
    int rangeIndex = 0;
    // A small buffer is used to minimize memory usage.
    char buffer[128];
    char* cp;
    int startingLocation = -1 ;  // return value -1 if no tzBlock is found

    while ( !(rangeIndex > endUsableRange)) {
        EEPROM.get(rangeIndex,buffer);
        cp = strstr(buffer, TZ_SIGNATURE);
        if (cp == NULL) {
            rangeIndex = rangeIndex + (sizeof(buffer) - sizeof(TZ_SIGNATURE));
        } else {
            startingLocation = rangeIndex + (cp - (char*)buffer);
            rangeIndex = startingLocation +1;
            // Note: We continue to search the entire EEPROM ... so logging exposes 
            //       multiple tzBlocks ... should they exist. 
            #ifdef LOGGING
                Serial.print("TzBlock found @ Location ");
                Serial.println(startingLocation);
            #endif
        }
    }
    return startingLocation;
}

// ---------------------------------------------------------------------------- updateDeviceSettings()
// Updates the devices local time settings based on the TzBlock stored in EEPROM  (object name =  tzEeprom)
void TzLib::updateDeviceSettings(void) {
    
    // called by tzSetup()
    // configures local time settings for Particle devices
    Time.zone(tzEeprom.stdOffset);
    Time.setDSTOffset(tzEeprom.curOffset - tzEeprom.stdOffset);
    if (Time.getDSTOffset == 0) {
        Time.endDST();
    } else {
        Time.beginDST();
    }
    
    #ifdef LOGGING
        Serial.println("\r\ntzLib>\tLocal Time Settings Complete ...");
        Serial.print("tzLib>\tTime.zone() ---> ");
        Serial.println(Time.zone());
        Serial.print("tzLib>\tTime.getDSTOffset() --> ");
        Serial.println(Time.getDSTOffset());
        Serial.print("tzLib>\tTime.isDST() ---> ");
        Serial.println(Time.isDST()? "true" : "false");
        Serial.print("tzLib>\ttzLib.getZone() ---> ");
        Serial.println(getZone());
        Serial.print("tzLib>\ttzLib.getZoneAbbr() ---> ");
        Serial.println(getZoneAbbr());
        Serial.print("tzLib>\tTime.format(Time.now(), TIME_FORMAT_DEFAULT) ---> ");
        Serial.println(Time.format(Time.now(), TIME_FORMAT_DEFAULT));
    #endif
    
    return;
}































