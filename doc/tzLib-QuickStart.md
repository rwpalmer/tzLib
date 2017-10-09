## tzLib Quick-Start Guide

### A typical / minimal implementation of tzLib 
```
    #include "tzLib.h"
	void Setup() {
		...
		tzSetDefaultZoneID(char*)"America/Chicago");
		tzSetup();
		...
	}
	void Loop() {
		...
		tzLoop();
		...
	}	
```		
### The functionality provided by the above code
	
#### tzSetDefaultZoneID 
    * Records the default time zone ID in memory
			
#### tzSetup
* Reads a block of data from EEPROM memory
* Queries the HTTP server to get time zone data
        * for the time zone registered in EEPROM memory or for the
          default time zone if no time zone is registered in EEPROM
    * Writes an updated data to EEPROM, if any new data was received
	  from the HTTP server
    * Configures the device's local time settings
        * based on query results if valid data was returned
        * based on EEPROM data if no valid data was returned and
          EEPROM data existed
        * to UTC if no data was returned for the default time zone
				
#### tzLoop
    * Triggers a DST transition when the scheduled time arrives
    * Triggers periodic EEPROM refreshes (every 3-9 weeks)
				   

				   
### Library Usage Notes:

    * tzLib relies on the HttpClient library to communicate with the
      HTTP server that provides time zone data, so:
           BOTH LIBRARIES MUST BE ADDED TO PROJECTs USING TZLIB
    * tzLib users do not need to do anything beyond adding the the
      HttpClient library to their project, but the HttpClient library
      is available to them if they have other communication needs
    * tzLib.h contains an "include" statement for the HttpClient library.
      User code MUST contain an "include" statment for tzLib, but an
	  "include" statment for the HttpClient library is optional. 

### HTTP Server Notes:

    * tzLib submits a PHP query to obtain time zone data, and tzLib.cpp
      contains default settings for the httpHost and httpPath variables
	  that point to the server that was brought online for library beta
	  testing.
    * If users like this library, the author's hope is that we will be
	  able to convince a commercial organization like Particle to provide
	  a server for production.
    * Anyone who wants to create their own server can easily do so. Such
      a server can be created as a virtual machine running on an existing
	  computer running Windows or Linux, so long as the computer has the 
	  required resources. This project provides a separate document that
	  discusses how the author created his test server. 
	
	
	