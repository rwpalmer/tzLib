/* Module: tzLib.cpp

Primary Functions:
    tzSetup():              Configures local time settings when the device boots
    tzLoop():               Performs DST time transitions when they are scheduled
    UpdateDeviceSettings(): Called internally to update device specific settings
    
Secondary Functions:
    tzGetInfo():            Returns the EEPROM object so firmware developers can use the data
    tzSetTransitionTime()   Allows developers to reset the DST transition time for testing transitions
    tzSGetSetupReturnMsg(): Returns the exit status message created by tzSetup():
    tzGetAbbr()             Returns the time zone abbreviation related to the current time zone settings
    tzGetTransitionAbbr():  Returns the time zone abbreviation related to the next DST transition
    tzSetDefaultZoneId():   Sets the default time zone ID
    tzWipeEEPROM            Erases tzLib data from the EEPROM
    tzSetEepromLocation():  Sets the location(offset) where tzLib will store data in EEPROM
    tzSetHttpHost():        Sets the host name or IP address of the server where tzLib data exists
    tzSetHttpPath():        Sets the path to the tzLib data on the server
*/

#include "tzLib.h"
//#define LOGGING true      // <-- For debugging, enable a serial console and remove the shashes at the beginning of this line
                                            // tzLoop() periodically calls tzSetup() to validate / refresh the tzInfo stored in EEPROM. 
const time_t tzRefreshInterval = 1723680;   // <-- This determines the interval between refreshes. (~3 weeks)
const time_t tzRetryInterval =  40000;      // <-- This determines the interval between retries if a refresh fails (~ 11 hours) 
static time_t tzRefreshTime;                // <-- this variable contains the epoch time for the next EEPROM refresh
static tzInfo_t eeprom;                     // <-- EEPROM settings are stored in this struct. for struct contents see tzLib.h
static int eepromLocation = 0;              // <-- Identifies the location in EEPROM where tzInfo should be stored
static char tzSetupReturnMsg[81];		    // <-- tzSetup stores a return status message here
static char defaultZoneId[65] = "UCT";	    // <-- "UCT" unless overridden with tzSetDefaultZoneId()
static char httpHost[129];                  // <-- The HTTP server's IP address or DNS name
static char httpPath[129];                  // <-- The path to time zone data on the HTTP server
static char eepromCurrentVersion[] = "01.00.01a";     // <-- the current ID used to identify the time zone data in EEPROM

HttpClient http;
http_response_t response;
http_request_t request;
http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { "Accept" , "application/json" },
    //{ "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};


// This Function is called by tzSetup. It configures local time settings on the device. 
// At some point in the future, this library might need to support devices that require
// different commands. In light of that eventuality, we chose to isolate device specific logic.

void UpdateDeviceSettings() {
    Time.zone(eeprom.stdOffset);
    float DSTOffset = (eeprom.currentOffset - eeprom.stdOffset);
    Time.setDSTOffset(DSTOffset);
    if (DSTOffset == 0) {
        Time.endDST();
    } else {
        Time.beginDST();
    }
    return;
}


int tzSetup(char* tzID) {
    
    tzInfo_t web; /* <-- This struct get's it's zoneId from the input arguement (tzID), or froom eeprom.zoneId when tzID is NULL.
                         Other data is populated via the web. If the web provides new data, the EEPROM will be updated. */

    // Get tzInfo from EEPROM --------------------
    EEPROM.get(eepromLocation,eeprom);
    if (strcmp(eeprom.version, eepromCurrentVersion)
    || (strlen(eeprom.zoneId) == 0)) {
        // EEPROM data is not valid, so we initialize eeprom
        strcpy(eeprom.version, eepromCurrentVersion);
        strcpy(eeprom.zoneId, defaultZoneId);
        eeprom.stdOffset = 0;
        eeprom.currentOffset = 0;
        eeprom.currentAbbr[0] = '\0';
        eeprom.transitionTime = 0;
        eeprom.transitionOffset = 0;
        eeprom.transitionAbbr[0] = '\0';
    }
            #ifdef LOGGING
                if (tzID == NULL) {
                    Serial.println("tzID Arguement is NULL");
                } else {
                    Serial.print("tzID Arguement = '");
                    Serial.print(tzID);
                    Serial.println("'");
                }
                Serial.print("   eeprom.version          = ");
                Serial.println(String(eeprom.version));
                Serial.print("   eeprom.zoneId           = ");
                Serial.println(String(eeprom.zoneId));
                Serial.print("   eeprom.stdOffset        = ");
                Serial.println(String(eeprom.stdOffset));
                Serial.print("   eeprom.currentOffset    = ");
                Serial.println(String(eeprom.currentOffset));
                Serial.print("   eeprom.currentAbbr      = ");
                Serial.println(String(eeprom.currentAbbr));
                Serial.print("   eeprom.transitonTime    = ");
                Serial.println(String(eeprom.transitionTime));
                Serial.print("   eeprom.transitionOffset = ");
                Serial.println(String(eeprom.transitionOffset));
                Serial.print("   eeprom.transitionAbbr   = ");
                Serial.println(String(eeprom.transitionAbbr));
                Serial.print("   Sizeof(eeprom) = ");
                Serial.println(String(sizeof(eeprom)));
            #endif
    if (tzID == NULL) {
        strcpy(web.zoneId, eeprom.zoneId);
    } else {
        strncpy(web.zoneId, tzID, sizeof(web.zoneId));
    }
 
 
    // Get tzOffsetJSON from HTTP Host
    char query[100] = "{\"timeZoneID\":\"";
    strcat (query, web.zoneId);
    strcat (query, "\"}");

            #ifdef LOGGING
                Serial.print("\nhttpHost = ");
                Serial.println(String(httpHost));
                Serial.print("httpPath  = ");
                Serial.println(String(httpPath));
                Serial.print("query = ");
                Serial.println(String(query));
            #endif
			
    tzSetupReturnMsg[0]='\0';
    request.hostname = httpHost;
    request.port = 80;
    request.path = httpPath;
    request.body = query;
    http.post(request, response, headers);
	
            #ifdef LOGGING
                Serial.println("request.body = " + String(request.body));
                Serial.print("\nhttp.status = ");
                Serial.println(String(response.status));
                Serial.print("response.body   = ");
                Serial.println(String(response.body));
            #endif
			
    if (response.status != 200) {
        UpdateDeviceSettings();
        if (response.status == 404) {
            strncpy(tzSetupReturnMsg, web.zoneId, sizeof(tzSetupReturnMsg));
            strncat(tzSetupReturnMsg, " is an invalid time zone ID", sizeof(tzSetupReturnMsg));
            return -1;
        } else if (response.status == -1) {
            tzRefreshTime = Time.now() + tzRetryInterval; // when tzSetup() fails to contact the HTTP server, this will trigger a retry
            strncpy(tzSetupReturnMsg, "Unable to connect to HTTP Server", sizeof(tzSetupReturnMsg));
            return -2;
        } else { 
            strncpy(tzSetupReturnMsg, "HTTP Server returned error '", sizeof(tzSetupReturnMsg));
            char str[10];
            strncat(tzSetupReturnMsg, itoa(response.status, str, 10), sizeof(tzSetupReturnMsg));
            strncat(tzSetupReturnMsg, "'", sizeof(tzSetupReturnMsg));
            return -3;
        }
    } else {    //  <-- http.status == 200
    
    //////////////////// The HTTP Server Reports Success, So We Will Parse the JSON Data /////////////////////
    
         /* This is an example of what the JSON should look like --- 
                {"StdOffset":-6.0,"CurrentOffset":-5.0,"NextTransition":{"EpochSeconds":1509865200,"Offset":-6.0}}
            Note: The NextTransition object is only present when the TZ implements DST
        */
    
        // Create and intitialize the tzInfo_t struct that will contain the parsed JSON data
        strcpy(web.version, eepromCurrentVersion);
        web.stdOffset = 0;
        web.currentOffset = 0;
        web.transitionTime = 0;
        web.transitionOffset = 0;
        
        // Parse the JSON Data in a non-elegant, but economical manner. 
        char* pch;
        bool parsingError = false;
        pch = strstr(response.body,"\"sOffset\":");
        if (pch != NULL) { 
            web.stdOffset = atof(pch+10);           
        } else {
            parsingError = true;
        }
        pch = strstr(response.body,"\"cOffset\":");
        if (pch != NULL) { 
            web.currentOffset = atof(pch+10);
        } else {
            parsingError = true;
        }
        pch = strstr(response.body,"\"cAbbr\":\"");
        if (pch != NULL) {
            char* qch = strchr(pch+9, '\"');    
            strncpy(web.currentAbbr, pch+9, (qch)-(pch+9));
        }
        pch = strstr(response.body,"\"tTime\":");
        if (pch != NULL) { 
            web.transitionTime = atol(pch+8);
        }
        pch = strstr(response.body,"\"tOffset\":");
        if (pch != NULL) { 
            web.transitionOffset = atof(pch+10);
        }
        pch = strstr(response.body,"\"tAbbr\":\"");
        if (pch != NULL) {
            char* qch = strchr(pch+9, '\"');
            strncpy(web.transitionAbbr, pch+9, (qch)-(pch+9));
        }
				#ifdef LOGGING
					Serial.print("    web.version          = ");
					Serial.println(String(web.version));
					Serial.print("    web.zoneId           = ");
					Serial.println(String(web.zoneId));
					Serial.print("    web.stdOffset        = ");
					Serial.println(String(web.stdOffset));
					Serial.print("    web.currentOffset    = ");
					Serial.println(String(web.currentOffset));
                    Serial.print("    web.currentAbbr      = ");
                    Serial.println(String(web.currentAbbr));
					Serial.print("    web.transitonTime    = ");
					Serial.println(String(web.transitionTime));
					Serial.print("    web.transitionOffset = ");
					Serial.println(String(web.transitionOffset));
                    Serial.print("    web.transitionAbbr   = ");
                    Serial.println(String(web.transitionAbbr));
				#endif

        if (parsingError) {
            UpdateDeviceSettings();
            strncpy(tzSetupReturnMsg, "JSON parsing failed", sizeof(tzSetupReturnMsg));
            return -4;
        } else {    //  <-- parsingError == false
            
        //////////////////// Parsing was successful, so it is time to use the data  /////////////////////

            int refresh_multiplier = 1;
            // If the tz has no DST scheduled transitions, we will trebble the refresh Interval
            if ((web.transitionTime == 0) && (web.stdOffset == web.currentOffset)) {
                refresh_multiplier = 3;
            } else {
                // Correct JSON data for any transitions that may have already taken place ...
                if (web.transitionTime < Time.now()) {
                    web.currentOffset = web.transitionOffset;
                    strcpy(web.currentAbbr, web.transitionAbbr);
                    web.transitionTime = 0;
                    web.transitionOffset = 0;
                    web.transitionAbbr[0]='\0';
                }
            }

            // If something changed, update the EEPROM
            if ((strcmp(eeprom.zoneId, web.zoneId) != 0)
            || (eeprom.stdOffset != web.stdOffset)
            || (eeprom.currentOffset != web.currentOffset)
            || (strcmp(eeprom.currentAbbr, web.currentAbbr) != 0)
            || (eeprom.transitionTime != web.transitionTime)
            || (eeprom.transitionOffset != web.transitionOffset) 
            || (strcmp(eeprom.transitionAbbr, web.transitionAbbr) != 0)) {
                EEPROM.put(eepromLocation,web);
                EEPROM.get(eepromLocation,eeprom);
                strncpy(tzSetupReturnMsg, "EEPROM Data Updated", sizeof(tzSetupReturnMsg));
            } else {
                strncpy(tzSetupReturnMsg, "EEPROM Data Verified", sizeof(tzSetupReturnMsg));
            }
            UpdateDeviceSettings();
            tzRefreshTime = Time.now() + (tzRefreshInterval * refresh_multiplier);
            return EXIT_SUCCESS;
        }
    }
}


void tzLoop() {
    
    // If it is time to transition local time to a different UTC offset ... 
    if ((eeprom.transitionTime != 0) && (eeprom.transitionTime <= Time.now())) {
        
        // Update tzInfo ... making the transition settings the current settings
        eeprom.currentOffset = eeprom.transitionOffset;
        strcpy(eeprom.currentAbbr, eeprom.transitionAbbr);
        eeprom.transitionTime = 0;
        eeprom.transitionOffset = 0;
        eeprom.transitionAbbr[0] = '\0';

       // Update the device to the transition settings
        float DSTOffset = (eeprom.currentOffset - eeprom.stdOffset);
        Time.setDSTOffset(DSTOffset);
        if (DSTOffset == 0) {
            Time.endDST();
        } else {
            Time.beginDST();
        }

        // Update the EEProm
        EEPROM.put(eepromLocation,eeprom);

        // Note: eeprom transition settings are simply cleared.
        //       tzInfo transition settings will be updated on the next reboot or tzInfo refresh.
    
        // If it is time to refresh tzInfo ...
        if (Time.now() >= tzRefreshTime) {
            tzSetup();
        }
    }
}

tzInfo_t tzGetInfo() {
    return eeprom;
}

void tzSetTransitionTime(time_t tTime) {
    eeprom.transitionTime = tTime;
}

char* tzGetSetupReturnMsg() {
    return tzSetupReturnMsg;
}

char* tzGetAbbr() {
    return eeprom.currentAbbr;
}

char* tzGetTransitionAbbr() {
    return eeprom.transitionAbbr;
}

void tzSetDefaultZoneId(char* id) {
    strncpy(defaultZoneId, id, sizeof(defaultZoneId));
}
int tzWipeEEPROM() {
    tzInfo_t eeprom;
	if (strcmp(eeprom.version, eepromCurrentVersion) == 0) {
		eeprom.version[0] = '\0';
		eeprom.zoneId[0] = '\0';
		eeprom.stdOffset = 0;
		eeprom.currentOffset = 0;
		eeprom.currentAbbr[0] = '\0';
		eeprom.transitionTime = 0;
		eeprom.transitionOffset = 0;
		eeprom.transitionAbbr[0] = '\0';
		EEPROM.put(eepromLocation,eeprom);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

void tzSetEepromLocation(int i) {
    eepromLocation = i;
}

void tzSetHttpHost(char* host) {
	strncpy(httpHost, host, sizeof(httpHost));
}

void tzSetHttpPath(char* path) {
	strncpy(httpPath, path, sizeof(httpPath));
}











