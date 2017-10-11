# tzLib


	- 	Configures a device's local time settings each time the device reboots.
	-	Performs transitions to and from daylight-savings time.
	-	Designed for use in IOT devices ... uses minimal resources.
	-	Extremely easy to implement ... Most implementations only require these
		three lines of code:
```cpp		
	   Setup() {
	      tzSetDefaultZoneId("<time zone id>"); // <- set default timezone
	      tzSetup();                            // <- configure local time   
	   }
		   
	   Loop() {
	      tzLoop();                           // <- perform DST transitions
	   }
```
		CAUTION: This library reads and writes to the devices EEPROM
		memory. If firmware uses EEPROM memory for other purposes, the 
		EEPROM offset may need to be adjusted. (See the library reference
		document) 

### HOW THE LIBRARY WORKS 

	-	IANA maintains the time zone database that communication companies
		and OS vendors use to manage local time world wide. 

	-	Using an IANA defined time zone ID as a key to the timezone database,
		tzLib submits a PHP query to an HTTP Server, and the HTTP Server
		responds with JSON file that contains six data elements:
			1.  The time zone's standard offset. <-- This never changes
			2.  The time zone's current offset. <-- Changes with DST
			3.  An abbreviation that describes the current offset
			4.  The time of the next DST transition <--Epoch Seconds UCT
			5.  The current offset after the next DST transition.
			6.  An abbreviation that describes the post transition offset.

	-	After downloading and parsing the JSON file, tzLib updates the devices
		time zone settings and stores the information that it has acquired in
		the devices EEPROM. This assures that the data will be available for 
		future reboots even if there is no network connectivity at that time.

	-	The above is all done in the Setup() portion of firmware. In the 
		firmwares loop() section, tzLib compares the current epoch time UCT to 
		the next DST transition time. When the time arrives, the transition
		will be performed. tzLib also compares the current epoch time UCT to a
		calculated refresh time. When that time arrives, fresh JSON data 
		will be downloaded, and the devices EEPROM will be updated if any new 
		data exists. Refreshing is performed ~ every three weeks for time zones
		with DST, and ~ every nine weeks for fixed time zones.
	

### CURRENT STATE OF THE PROJECT

	-	Library (0.1.0) exited Alpha Testing with no known bugs on 8 Oct, 2017
	-	The library has only been tested on the Particle Photon. The author is
		relying on the community to test on other devices. Please report any
		test results.
	-	Documentation has not been reviewed for spelling or grammer, and some
		inaccuracies may exist. 
	-	An HTTP server has been brought up on the web to support beta testing.
	

### ISSUES
		
	-  The author is still searching for someone willing to host the PHP on a
	   data-center quality server. Any leads or contacts would be appreciated.
	   

Many thanks to Kasper Kamperman for suggesting the move from Java to PHP, and
for helping to assemble the PHP code used in this project.





