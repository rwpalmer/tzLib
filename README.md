# tzLib

## Automatically maintains the device's local time settings.

### So, what does "Automatically" mean?
tzLib does not sense the device's time zone, but ...
*	tzLib will configure local time based on a firmware defined "default time zone ID", or "Olson Name".
*	Particle makes it possible to Accept time zone changes from the web.
*	tzLib performs time zone changes in real-time, and remembers the current timezone when the device reboots.
*	tzLib should be able to accept time zone changes from geopositioning software as well, but that has yet to be proven.
*	In the mean time, we can provide device users with a list-based or map-based time-zone selection tool. A number of choices are available on GitHub, and elsewhere on the Internet. The only requirement is that they generate a time zone ID that can be found in IANA's time zone database.
*	Oh, and yes, tzLib does automatically perform DST transitions.  




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

*__I M P O R T A N T__*  
tzLib will store data in the devices EEPROM. By default EEPROM bytes 0-127 are used. 
* 	The command "tzLib.setEepromStartByte(0);" reflects that default.  
* 	If the default location is not available, developers must modify this command and insert it between #1 and #2 in the code sample.
*	For example, "tzLib.setEepromStartByte(512);" would tell tzLib to use EEPROM bytes 512-639. 
*	Please see the tzLib-QuickStart guide in the doc folder of the projects GitHub repository for more details.




## HOW THE LIBRARY WORKS 

* IANA maintains the time zone database that communication companies and OS vendors use to manage local time world wide. 

* Using an IANA defined time zone ID as a key, tzLIb issues a PHP query to an HTTP server to obtain the time zone data it needs to configure local time and to perform DST transitions at the proper time. 

* Time zone data is stored in EEPROM to assue that the data is available whenever the system reboots, even if no network connection is available at that time. 



## CURRENT STATE OF THE PROJECT

tzLib (0.1.1) will exit alpha testing soon.
*	Known defects: none as of 9 Nov 2017
	
Code and documentation are under review. 
	
To support beta testing, an HTTP server has been connected to the  web, and the library has been preconfigured to use this server by default. 
		
For those who wish to create their server, or to use an alternate server. 
* the library includes methods to configure host name, path, and port.
* this project's GitHub repository includes:
	* 	the getJson.php file that tzLib needs to run on the HTTP server.
	* 	a document named tzLib_ServerBuild.md,  which contains details about how the author created the virtual HTTP server that he used for alpha testing. Such a server can run as a guest operating system within most Windows, Linux, or Apple systems. 

The search for someone to host the production HTTP server is underway.  We hope to find a corporate entity who can host the PHP file in their data-center, so library users will have a platform that they can rely upon. Please contact the author if you can help, or if you have any contacts who might be able to help in this effort. 
		
Hosting requirements are minimal: 
	* 	Host a small PHP file (332 bytes) on an HTTP server.
	* 	Keep the server's time zone database current.
	* 	Provide production level availability.
		
Please report any issues, suggestions, and comments to the author and maintainer: rwpalmeribm@gmail.com
	   

Many thanks to Kasper Kamperman for suggesting the move from Java to PHP,
and for helping to assemble the PHP code used in this project.





