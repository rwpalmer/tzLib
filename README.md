# tzLib


* Configures the device's local time settings based upon:
	*	a configured time zone ID.
	*	time zone details obtained from an HTTP server.
* Performs Daylight Savings Time transitions when they are scheduled.
* Designed for use with IOT devices.
* Supports user initiated time zone selection via the web.
* Should support time zone changes triggered by geopositioning software. (not proven yet)
* Easy to implement ...

Most implementations only require the addition of these four lines of code:

```cpp		
	   Setup() {
	      tzLib.begin();                         	// <- 1. Prepare tzLib to run

	      tzLib.setDefaultZone("<timezone id>"); 	// <- 2. Set default timezone

	      tzLib.setLocalTime();                  	// <- 3. configure local time   
	   }
		   
	   Loop() {
	      tzLib.maintainLocaltime();          // <- 4. perform DST transitions & keeps time zone data current.
	   }
```

*__WARNING__*  
tzLib stores 128 bytes of time zone data in the devices EEPROM. This 128 byte data block is called the tzBlock.
* 	Default tzBlock location:  EEPROM byte 0
* 	If the default location is used for other purposes, developers must use tzLib.setEepromStartByte() to select another location, or tzLib s
*	eg: tzLib.setEepromStartByte(512);



## HOW THE LIBRARY WORKS 

- IANA maintains the time zone database that communication companies and OS vendors use to manage local time world wide. 

- Using an IANA defined time zone ID as a key to the timezone database, tzLib submits a PHP query to an HTTP Server, and the HTTP Server responds with six data elements:
	1.	The time zone's standard offset. 		
	1.	The time zone's current offset.		
	1.	An abbreviation that describes the current offset.
	1.	The time of the next DST transition.
	1.	The current offset after the next DST transition.
	1.	An abbreviation that describes the post transition offset.

- Using the data from the HTTP server, tzLib updates the devices time zone settings and stores a 128-byte tzBlock in EEPROM. The tzBlock retains:
	- The time zone ID, so tzLib can periodically query the HTTP server to keep time zone data current.
	- Server provided data elements required to configurelocal time, so that tzLib can configure local time whenever the system reboots, even if no network connectivity is available at that time.
	- DST transition data to assure that tzLib can perform DST transitions at the scheduled time. 
		


## CURRENT STATE OF THE PROJECT

tzLib (0.1.1) will exit alpha testing soon.
-	Known defects: none as of 9 Nov 2017
	
Documentation is still being written.
	
To support beta testing, an HTTP server has been connected to the  web, and the library has been preconfigured to use this server by default. 
		
For those who wish to create their server, or to use an alternate server. 
- the library includes methods to configure host name, path, and port.
- this project's GitHub repository includes:
	- the getJson.php file that tzLib needs to run on the HTTP server.
	- a document named tzLib_ServerBuild.md,  which contains details about how the author created the virtual HTTP server that he used for alpha testing. Such a server can run as a guest operating system within most Windows, Linux, or Apple systems. 

The search for someone to host the production HTTP server is underway.  We hope to find a corporate entity who can host the PHP file in their data-center, so library users will have a platform that they can rely upon. Please contact the author if you can help, or if you have any contacts who might be able to help in this effort. 
		
Hosting requirements are minimal: 
	- Host a small PHP file (332 bytes) on an HTTP server.
	- Keep the server's time zone database current.
	- Provide production level availability.
		
Please report any issues, suggestions, and comments to the author and maintainer: rwpalmeribm@gmail.com
	   

Many thanks to Kasper Kamperman for suggesting the move from Java to PHP,
and for helping to assemble the PHP code used in this project.





