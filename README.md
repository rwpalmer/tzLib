# tzLib
Manages local time settings and DST transitions for IOT devices.

	----------------------------- Background ---------------------------------- 

	Local time settings are a function of geography and the policy of the
	governments that rule over that geography. Any government body (be it a
	nation, a state or province, a county, or even a city can decide that 
	resetting the clocks in their domain would make life better for
	the citizenry working and residing there. Common reasons for such a
	determination relate to agriculture, safety, and commerce. Regardless of
	the reason, a government can adopt a different time zone or use daylight 
	savings time (DST) to reset their clocks. The point is: Local time is not 
	static. It can, and does change from time to time.
	
	In addition to administering internet domain names and IP addresses, the 
	IANA (Internet Assigned Numbers Authority) maintains a time zone database
	that is used by communications and computer companies to administer local
	time world wide. The database is a collection of rules that specify how
	to calculate local time for a geography given a specific date in time, 
	be it in the past, the present, or the future. This database is updated
	periodically, often multiple times a year.  If you want more information
	please go to www.iana.org.
	
	IANA's database is the most authoritative database around, and it would be 
	possible to write a program to access it directly. However, that program
	would be incredibly complex, and such a program would NEVER fit on an IOT 
	device. To create this library, we needed to find a way to extract the
	data that IOT devices need, and to make it available to them.
	
	Linux and other OS vendors, like Microsoft and Apple have done the heavy
	lifting for us. They use the data for their own time management, and to
	to support time libraries for application developers.  Even so, 
	extracting the data needed for IOT devices was still a daunting task until
	Oracle released Java 8 and ZoneRules. With the ZoneRules library, we can 
	easily extract the IANA data that IOT devices need. 



	---------------- How does tzLib acquire the data required -----------------
	----------------- to manage local time on an IOT device? ------------------
	
	A Java program generates extracts IANA data from the OS it runs on to
	answer the following questions:
	
		What is the standard time zone offset for my time zone?
		What is the current time zone offset for my time zone?
		When is the next DST transition for my time zone?
		What will the time zone offset be after the next DST transition?
	
		The returned data can be used to answer other questions for example:
		-	"What is the current DST Offset?": can be computed by subtracting 
			the standard offset from the current offset.
		-	"Is the current time STD or DST?": can be computed by comparing 
			the current offset to the standard offset. When the two match,
			time is STD. When they differ, time is DST. 
	
	
	
	------------------ How do IOT devices obtain the data? --------------------
	
	The Java program formats the data it extracts into JSON files. One file is
	generated for each time zone, and the name of the file is the IANA defined
	time zone ID.

	tzLib GETs the JSON data with the help of another library  "HttpClient",
	which uses economical and well tested code.

	///////////////////////////////////////////////////////////////////////////

								IMPORTANT NOTE

	At the present time, there is no HTTP server hosting the tzLib JSON
	files on the internet. I am searching for an IOT vendor or another company 
	who has a real interest in IOT to host the data. Please let me know if you
	have any contacts.
	
	In the mean time, and for those want/need to have their own HTTP server, I
	can offer three options. From easiest to most complicated ... 
	
		1. 	You can install Oracle's Virtual Box on a Windows or Linux system,
			download the .OVA file containing an image of the virtual machine
			that hosts my Ubuntu Linux/Apache HTTP Server, fire it up, and make
			a few minor tweaks related to password and the IP address. 
			
			OVA is an open format, so my virtual machine should also run on
			other hypervisors (like VMware, KVM, or HyperV) but no testing has
			been done.
			
		2.	You can create your own HTTP server, and use the .JAR file to
			to populate the JSON data. 
			
		3.	You can start with the Java program, create your own .JAR file,
			and go on from there. If you do this, make sure you use Java 8
			or above. 
			
	///////////////////////////////////////////////////////////////////////////
	
	
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
