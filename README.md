# tzLib

## Automatically maintains the device's local time settings.

### So, what does "Automatically" mean?
tzLib does not automatically set a device's local time based upon the device's physical location ... but it does make it possible for developers and users to change the device's time zone in real time based on web-based triggers or geopositioning software.
*	On first boot, tzLib will configure local time based on a firmware defined "default time zone ID", or "Olson Name".
*	Once running, a Particle function can be used to accept input from the web, and that function can call tzLib to change the device's time zone. This capability is demonstrated in one of tzLib's firmware examples. 
*	This functionality also means that developers can provide device users with a list-based or map-based time-zone selection tool to change the device's time zone. A number of such tools are available on GitHub, and elsewhere on the Internet. The only requirement is that the tool must generate time zone IDs that can be found in IANA's time zone database.
*	Oh, and yes, tzLib does automatically perform DST transitions.  


## How the Library Works ...

* IANA maintains the time zone database that communication companies and OS vendors use to manage local time world wide. 

* Using an IANA defined time zone ID as a key, tzLIb issues a PHP query to an HTTP server to obtain the time zone data it needs to configure local time and to perform DST transitions at their scheduled time.

* Time zone data is stored in EEPROM to assue that the data is available whenever the system reboots, even if no network connection is available at that time. 

## Sample Usage
#### Most implementations require the addition of four lines of code. This sample includes a fifth to make it clear when the fifth line must be included.

```cpp		
	setup() {
	    tzLib.begin();                          // <- 1. Prepare tzLib to run

	    tzLib.setEepromStartByte(0);            // <- 2. Tell tzLib where to store data in EEPROM		  
		  
	    tzLib.setDefaultZone("<timezone id>");  // <- 3. Set the default time zone

	    tzLib.setLocalTime();                   // <- 4. Configure local time   
	}
		   
	loop() {
	    tzLib.maintainLocaltime();       // <- 5. Perform DST transitions & keep time zone data current.
	}
```

*__I M P O R T A N T__*  
* 	The command "tzLib.setEepromStartByte(0);" reflects the tzLib default setting, and can be omitted in most implementations.
* 	If the default location is not available for tzLib to use, this command must be modified and left in place. 
*	For example, "tzLib.setEepromStartByte(512);" would tell tzLib to use EEPROM bytes 512-639. 
*	Please see the tzLib-QuickStart guide on GitHub for more details. https://github.com/rwpalmer/tzLib/blob/master/doc/tzLib-QuickStart.md


##	Library Documentation
tzLib documentation includes an Overview, a QuickStart Guide, a Reference Guide, and a manual that explains how the author created the HTTP server he used for testing tzLib. https://github.com/rwpalmer/tzLib/tree/master/doc


##	Firmware Examples
tzLib is packaged with three firmware examples. All three are designed to be run with a serial console.
*	*tzLib101.ino* demonstrates setting local time and displaying local time variables exposed by tzLib and the Particle Time class.
*	*tzLib201.ino* extends txLib101 by performing a test DST transition and reprinting the local time variables.
*	*tzLib301.ino* extends tzLib101 in a different direction ... <img align="right" src="tzLib301.png"> demonstrating how the device's time zone can be changed via the Particle Console and via the web. To the right, you see how the Particle Function and Particle Variables used in tzLib301 are displayed on the Particle Console. Type a valid time zone ID into the Function Arguement field and click "CALL" to change time zone. Click "Get" to display each new variable setting. 

Sample "serial console output" is included in the comments at the the bottom of each example's .ino file. tzLib301.ino comments includes HTML that can be used to trigger a time zone change on the device. https://github.com/rwpalmer/tzLib/tree/master/examples


## Current State of the Project ...

tzLib (0.1.1) will exit alpha testing soon.
*	Known defects: none as of 11 Nov 2017
	
Code and documentation are under review. 
	
To support beta testing, an HTTP server has been connected to the  web, and the library has been preconfigured to use this server by default. 
		
For those who wish to create their server, or to use an alternate server. 
* the library includes methods to configure host name, path, and port.
* this project's GitHub repository includes:
	* 	the getJson.php file that tzLib needs to run on the HTTP server.
	* 	a document named tzLib_ServerBuild.md,  which contains details about how the author created the virtual HTTP server that he used for alpha testing. Such a server can run as a guest operating system within most Windows, Linux, or Apple systems. 

The search for someone to host the production HTTP server is underway.  We hope to find a corporate entity who can host the PHP file in their data-center, so library users will have a platform that they can rely upon. Please contact the author if you can help, or if you have any contacts who might be able to help in this effort. 
		
Hosting requirements are minimal: 
	* 	Host a small (3KB) PHP file on an HTTP server.
	* 	Keep the server's time zone database current.
	* 	Provide production level availability.
		
Please report any issues, suggestions, and comments to the author and maintainer: rwpalmeribm@gmail.com
	   

Many thanks to Kasper Kamperman (https://www.kasperkamperman.com) for suggesting the move from Java to PHP,
and for helping to assemble the PHP code used in this project.





