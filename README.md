# tzLib


THIS LIBRARY 

	- 	Configures a device's local time settings each time the device reboots.
	-	Performs transitions to and from daylight-savings time at the
		appropriate time.
	-	Is designed for implementation in firmware.
	-	Consumes very few device resources
	-	Uses time zone offset and DST transition data that has been prestaged
		on an HTTP Server. 


HOW THE LIBRARY WORKS 

	-	IANA maintains the time zone database that communication companies
		and OS vendors use to manage local time world wide. This project uses
		a Java program to tap into this database. This Java generates a JSON
		file for every IANA defined time zone (over 600 worldwide). The JSON 
		files are written directly to an HTTP Server, and these files are
		refreshed daily. 
	-	Using an IANA defined time zone ID, tzLib retrieves the JSON file 
		that is appropriate for the device. The JSON file provides four
		data elements:
				1. 	The time zone's standard offset. <-- This never changes
				2.	The time zone's current offset. <-- Changes with DST
				3.  The time of the next DST transition <--Epoch Seconds UCT
				4.  The current offset after the next DST transition.
	-	After downloading and parsing the JSON file, tzLib updates the devices
		time zone settings and stores the time zone ID along with the 
		downloaded data elements in the devices EEPROM. This assures that the
		data will be available for future reboots even if there is no
		network connectivity at that time. 
	-	The above is all done in the Setup() portion of firmware. In the 
		firmwares loop(), tzLib compares the current epoch time UCT to the next
		DST transition time. When that time comes, the transition will be
		performed. tzLib also compares the current epoch time UCT to a refresh
		calculated refresh time. When that time comes, fresh JSON data will
		be downloaded, and the devices EEPROM will be updated if new data 
		exists. Refreshing is performed ~ every three weeks for time zones
		with DST, and ~ every nine weeks for fixed time zones.
	

CURRENT STATE OF THE PROJECT

	-	Library components and example code are in the final stages of alpha
		testing.
	-	No known bugs exist at this time.
	-	Code reviews with other developers have not been performed. 
	-	The library has only been tested on the Particle Photon.
	-	Documentation has not been reviewed for spelling, grammer or accuracy.
	-	Testing has only been performed with an HTTP Server running in a
		virtual machine that is hosted on the developers laptop via Oracle's 
		Virtual Box. 
	-	NO HTTP SERVER HOSTS THE REQUIRED JSON FILES ON THE INTERNET AT THIS
		TIME, but the author can provide an OVA file so testers can run a copy
		of the HTTP server he is using. 
	-	If any one is willing to help with the project, please contact the 
		author/maintainer. 





