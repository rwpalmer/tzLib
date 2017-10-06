# tzLib


	#### tzLib manages local time settings and DST transitions for IOT devices


## Background:

	Local time settings are a function of geography and the policy of the 
	governments that rule over that geography.  Any government body (be it 
	a nation, a state or province, a county, or even a city can decide that 
	resetting the clocksin their domain would make life better for the 
	citizenry working and residing there. Common reasons for such a 
	determination relate to agriculture, safety, and commerce. Regardless 
	of the reason, a government can adopt a different time zone or use daylight 
	savings time (DST) to reset their clocks. The point is: Local time is not 
	static. It can, and does change from time to time.
	
	In addition to administering internet domain names and IP addresses, the
	IANA (Internet Assigned Numbers Authority) maintains a time zone database 
	that is used by communications and computer companies to administer local 
	time world wide. The database is a collection of rules that specify how to 
	calculate local time for a geography given a specific date in time,
	be it in the past, the present, or the future. This database is updated 
	periodically, often multiple times a year.If you want more information
	please go to http://www.iana.org.
	
	IANA's database is the most authoritative database around, but this is a 
	__rules-based__ database which needs to be converted to data that computers
	can readily use. 

	tzLib runs a PHP query on an HTTP server to obtain the timezone data that is 
	required to maintain the IOT devices local settings and to perform DST 
	transitions at the proper time. PHP uses IANA rules to create the database 
	that we access. https://pecl.php.net/package/timezonedb.




### How does tzLib acquire the data required to manage local time settings?
	
	tzLib leverages the HttpClient library to submit an HTTP POST to an HTTP
	Server. This POST provides the server with the device's time zone ID, and
	triggers a PHP script to run on	the server. When the PHP script exits, the 
	answers to six questions are returned to the tzLib.
	
		1. What is the standard time zone offset the submitted time zone ID?
		2. What is the current time zone offset for the submitted time zone ID?
		3. What is the abbreviation associated with the current offset?
		4. When is the next DST transition for the submitted time zone ID?
		5. What will the time zone offset be after the next DST transition?
		6. What is the abbreviation associated with the post transiton offset?
	
		The returned data can be used to answer other questions for example:
		-	"What is the current DST Offset?": can be computed by subtracting 
			the standard offset from the current offset.
		-	"Is the current time STD or DST?": can be computed by comparing 
			the current offset to the standard offset. When the two match,
			time is STD. When they differ, time is DST. 
	
	
	
	
		----------------- What does tzLib do with the data? -------------------

	Once tzLib has received the JSON file, tzLib parses the JSON and updates 
	the devices EEPROM if any new data is received. The devices local time 
	settings are then updated. 
		For the Particle Photon, this involves setting
			Time.zone() is set to the current offset
			Time.setDSTOffset() is set to ( the current - the standard offset)
			Time.endDST () is invoked if DSTOffset is 0
			Time.beginDST() is invoked DSTOffset is not 0
	
	For time zones that have DST, tzLib will also reset the local time settings
	when the next DST transition time is reached.
	
	Since it may be a long time between reboots, tzLib validates EEPROM data
	periodically. This will assure that EEPROM data accurately reflects any
	IANA database changes, and that DST transition data is updated after each
	transition occurs. 



	----- How does tzLib know what time zone ID to submit to the server? -----

	With no other guidance, tzLib will submit UTC (aka GMT, or Zulu).

	The firmware developer can use a tzLib method to assign a default time
	zone ID, and the firmware developer can create a Particle function to
	employ another tzLib method to change the time zone from its default value.

	If a Particle function is provided the time zone ID can be changed based 
	on input from external sources. For example, there are a number of HTTP
	based time-zone selection tools on the web. Some of these are list based,
	others are map based. It would also be possible to use geolocation
	software to automatically trigger time zone changes as devices move around
	the planet. 