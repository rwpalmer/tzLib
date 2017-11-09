#ifndef __TZLIB_H_
#define __TZLIB_H_
#include "application.h"
//#define LOGGING true      // <-- true for debugging, false (or commented out) For production

const time_t tzBlockRefreshInterval = 1723680;  // <-- Specifies the interval between refreshes. (~3 weeks)
const time_t tzBlockRetryInterval =  40000;     // <-- Specifies the interval between retries if a refresh fails (~ 11 hours)
const char TZ_SIGNATURE[10] = "#!#TZ001a";      // <-- Used to identify the TzBlock in EEPROM.

// ------------------------------------------------------------------- tzBlock Class
// Defines the data that TzLib stores in EEPROM

class TzBlock {
	private:
	    // variable declaration
    	    char signature[10];                 //  <-- Identifies the TzBlock in EEPROM
    	    char id[65];                        //  <-- Selected time zone ID 
    		float stdOffset;                    //  <-- Standard offset for the selected time zone
    		float curOffset;                    //  <-- Current offset for the selected time zone
    		char curAbbr[6];                    //  <-- Current Abbreviation used for the selected time zone       
    		time_t tranTime;                    //  <-- Date/time for the next DST transition
    		float tranOffset;                   //  <-- Post-transition offset for the selected timezone
    		char tranAbbr[6];                   //  <-- Post-transition abbreviation for the selected timezone 
    		byte futureUse[20];                 //  <-- Reserved for future use
	    // constructor declaration --------------
    	    TzBlock();
		// method declaration ---------
    		bool equals(TzBlock tz);            // <-- Compares the contents of two TzBlocks
    		int applyJson(char* jsonStr);       // <-- Applies a server-supplied JSON string to a TzBlock
    		void log(char*);                    // <-- Displays the contents of a TzBlock on the Serial console (when LOGGING)
		
		friend class TzLib;
		friend class Json;
};

/*  TzBlock will be instantiated as follows:
        tzEeprom  ... will be instantiated as TzLib::tzEeprom, and represents the TzBlock stored in EEPROM
        tzWeb ...     will be instantiated in TzLib::setLocalTime and represents the TzBlock that is built
                      from the information provided by an HTTP server.
        Other methods also create TzBlocks for temporary use. 
*/
// ------------------------------------------------------------------- TzLib Class
// Defines tzLib core data and references
class TzLib {
    private:
        TzBlock tzEeprom;                           // <-- tzBlock object that stores time zone data in EEPROM
        bool tzEepromExists;                        // <-- Indicates if the tzBlock currently exists in EEPROM
        int eepromStartByte;                        // <-- The starting location of the tzBlock in EEPROM
        time_t eepromRefreshTime;                   // <-- Specifies when the next tzBlock refresh will take place
        char hostName[129];                         // <-- DNS name or IP address of the HTTP Server
        char hostPath[129];                         // <-- Location of the getJSON.php file on the HTTP Server (Start with '/')
        uint16_t hostPort;                          // <-- Host port number to use for HTTP (default 80)
        char defaultZone[65];                       // <-- The default time zone  (originally set to UTC, but may be overridden by tzLib.setDefaultZone()
        char statusMsg[65];                         // <-- Records processing status messages
    public:
        void begin(void);                           // <-- TzLib constructor renamed so users have placement options
        int setLocalTime(char* = NULL);             // <-- Sets the devices local time settings
        void maintainLocalTime(void);               // <-- Maintains the devices local time settings
        void setDefaultZone(char* id);              // <-- Sets the default time zone ID
        char* getZone(void);                        // <-- Gets the current time zone ID
        char* getZoneAbbr(void);                    // <-- Gets the current time zone Abbreviation
        void changeZone(char* id);                  // <-- Changes the current time zone ID and device settings
        void setEepromStartByte(int sb);            // <-- Sets the location of the tzBlock in EEPROM
        void setHostName(char*);                    // <-- Sets the HTTP server's host name or IP address
        void setHostPath(char*);                    // <-- Sets the HTTP server's path to the getJSON.php
        void setHostPort(uint16_t port);            // <-- Sets the HTTP server's port number (default 80)
        int setNextTransitionTime(time_t time);     // <-- Allows testers to schedule test DST transitions
        void transitionNow(void);                   // <-- For testing: Performs a a pending transition instantly 
		void eraseTzEeprom(void);                   // <-- Overwrites a TzBlock in EEPROM with '0xFF'
		char* getHttpStatus(void);                  // <-- Gets the HTTP Status Message


    private:
        void updateDeviceSettings(void);            // <-- Updates the device's local time settings
        int searchForTzEeprom(void);                // <-- Searches EEPROM for the presence of a TzBlock
};

//      TzLib is instantiated here, as a static object
static TzLib tzLib;



// ------------------------------------------------------------------- Json Class
// Provides methods used to encode and decode JSON
class Json {
    private:
        Json();
        void encodeQuery(char* json, char* tzId);               // <-- Encodes the JSON sent to the HTTP Server
        int get(float& n, char* json, char* name);              // <-- Gets a floating-point value for the associated JSON name
        int get(time_t& n, char* json, char* name);             // <-- Gets a time_t value for the associated JSON name
        int get(char* ca, int caSize, char* json, char* name);  // <-- Gets a character array for the associated JSON name

        friend int TzLib::setLocalTime(char*);
        friend int TzBlock::applyJson(char*);
};

// Json is instantiated in:
//  tzLib.cpp --- TzLib::setLocalTime(), where JSON is encoded
//  tzBlock.cpp --- TzBlock::applyJson(), where JSON is decoded



// ------------------------------------------------------------------- Http Class
// Handles communication with the HTTP server
class Http {
    private:
        TCPClient client;                   // <-- Stores the TCPClient object used to perform the HTTP POST transaction
        char buffer[512];                   // <-- Buffer where the HTTP server's response is stored
        bool error;                         // <-- Error flag used while processing HTTP
//        char errorMsg[65];                  // <-- Error message that is recorded while processing HTTP
        unsigned int bufferIndex;           // <-- Index to the buffer defined above
        unsigned long lastReadMillis;       // <-- Time when the last character was received from the host
        unsigned long startMillis;          // <-- Time when the POST transaction was sent to the server

        Http();
        int getJson(char* hostName, int hostPort, char* hostPath, char* jsonStr, int jsonSize, char* errorMsg, int errMsgSize);  // <-- Performs the HTTP processing
        
        friend int TzLib::setLocalTime(char*);
};

// Http is instantiated in tzLib.cpp --- TzLib::setLocalTime()

#endif






















