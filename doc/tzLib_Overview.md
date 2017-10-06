# tzLib

#### A library to manage local time settings and DST transitions for IOT devices


## Background:
	Local time settings are a function of geography and the policy of the 
	governments that rule over that geography.  Any government body (be it 
	a nation, a state or province, a county, or even a city) can decide that 
	resetting the clocks in their domain would make life better for the 
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
	'rules based' database which needs to be converted to data that computers
	can readily use. 

	tzLib runs a PHP query on an HTTP server to obtain the timezone data that is 
	required to maintain the IOT devices local settings and to perform DST 
	transitions at the proper time. PHP uses IANA rules to create the database 
	that we access. https://pecl.php.net/package/timezonedb.




### How does tzLib acquire the data required to manage local time settings?
	
	tzLib leverages the HttpClient library to submit an HTTP POST to an HTTP
	Server. This POST provides the server with the device's time zone ID, and
	triggers a PHP script to run on	the server. When the PHP script exits, 
	data related to the submitted time zone ID is returned to tzLib. This
	includes:
	
		-   Standard time zone offset
		-   Current time zone offset
		-   Abbreviation associated with the current offset
		-   Time for the next DST transition
		-   Post transition current offset
		-   Abbreviation associated with the post transition current offset
	
		The returned data can be used to compute:
		-   Current DST Offset = (current offset - standard offset)
		-   isDST = false when (current offset = standard offset)
		    isDST = true when (current offset != standard offset)
	
	
	
	
### What does tzLib do with the data?

	Once tzLib has received the JSON file, tzLib parses the JSON and updates 
	the devices EEPROM if any new data is received. The devices local time 
	settings are then updated. 
		For the Particle Photon, this involves setting
		   -   Time.zone() is set to the current offset
		   -   Time.setDSTOffset() is set to (current - the standard offset)
		   -   Time.endDST () is invoked if DSTOffset is 0
		   -   Time.beginDST() is invoked DSTOffset is not 0
	
	For time zones that have DST, tzLib will also reset the local time settings
	when the next DST transition time is scheduled.
	
	Since it may be a long time between reboots, tzLib validates EEPROM data
	periodically. This will assure that EEPROM data accurately reflects any
	IANA database changes, and that DST transition data is updated after each
	transition occurs. 



### How does tzLib know the correct time zone ID to submit to the server?

	With no other guidance, tzLib will default to UTC (aka GMT, or Zulu). 
	Firmware developers have the option of overriding this default.
	
	tzLib will adopt the default time zone ID on the first boot. The ID will
	be stored in EEPROM, and the device settings will be configured to 
	reflect the default time zone's standard offset and DST settings. 
	
	tzLib includes a method that can change the time zone ID and reset the 
	devices local time settings at any time. This method can be invoked from 
	an external internet source (via a Particle function), or by logic within
	the firmware loop itself. 
	
	Developers can provide a time-zone selection tool to their users. A number
	of list-based and map-based tools are available. For mobile devices, a
	geopositioning tool could also be used to change time zones as the device
	moves from one zone to another. The only restriction is that selection
	tools MUST generate a valid IANA time zone ID (aka Olson name).
	
