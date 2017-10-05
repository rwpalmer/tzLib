
tzLib_201 is an advanced code sample. It is designed to answer the following
questions:
	-	How can the the time zone ID be changed while the device is running?
	-	How can we emulate DST changes for application testing?

You will find two serial monitor captures below. In the first capture, we opt
to change time zone. That gives us a list of time zones to choose from. The 
capture shows that we selected time zone t, which is Australia/Tasmania. 
That time zone has a DST transition scheduled for 2AM Sunday October 1st, 2017.
At that time, they will move from standard time to DST. 

In the second capture, we opt to perform that DST transition NOW. The top portion 
of the capture shows how the settings were before the transition, and the
second half shows the settings after the transition. 

Note that there are no DST transitions pending after the transition. After each
transition occurs, the following transition time will be recalculated and
posted to the HTTP server. tzLib will then download the new date and post it
into EEPROM. This will be done on a routine data refresh cycle, or when the 
device is rebooted. 

Note: The bottom time zone ID listed in the time zone selection menu is
invalid. That is intentional ... for testing purposes. 




<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< BEGIN SERIAL MONITOR CAPTURE 1 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

Press 'q' to query EEPROM, 'c' to change time zone, or 't' to transition now: c
---------------------------------------------------------- TIME ZONE SELECTION ---
   a = Africa/Johannesburg
   b = Brazil/East
   c = America/Chicago
   d = Asia/Dhaka
   e = Asia/Saigon
   f = America/St_Johns
   i = Asia/Tehran
   j = Japan
   k = America/Indiana/Knox
   m = Pacific/Marquesas
   n = Canada/Newfoundland
   p = Europe/Amsterdam
   s = Singapore
   t = Australia/Tasmania
   v = Asia/Tel_Aviv
   x = Invalid
   z = Pacific/Chatham

   You may select one of the above timezones, or press esc to return: t
------------------------------------------------------------- TIME ZONE CHANGED ---
Australia/Tasmania
         Current Offset: 11.000000 hours, Standard Offset: 10.000000 hours
     Next DST Transiton:  1522512000 (epoch seconds UCT) = Sun Apr  1 03:00:00 2018 (Local Time)
      Transition Offset: 10.000000 hours
            EEPROM Data Updated
Device Settings: - - - - - - - - - - - - - - - - - -
            Time.zone() = 10.000000           Time.isDST() = 1
    Time.getDSTOffset() = 1.000000 hours
    The current time in this time zone is: Fri Oct  6 05:54:16 2017   AEDT
-----------------------------------------------------------------------------------
Press 'q' to query EEPROM, 'c' to change time zone, or 't' to transition now:

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< END SERIAL MONITOR CAPTURE 1 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>







<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< BEGIN SERIAL MONITOR CAPTURE 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

Press 'q' to query EEPROM, 'c' to change time zone, or 't' to transition now: t
----------------------------------------------- SETTINGS BEFORE DST TRANSITION ---
Australia/Tasmania
         Current Offset: 11.000000 hours, Standard Offset: 10.000000 hours
     Next DST Transiton:  1522512000 (epoch seconds UCT) = Sun Apr  1 03:00:00 2018 (Local Time)
      Transition Offset: 10.000000 hours
Device Settings: - - - - - - - - - - - - - - - - - -
            Time.zone() = 10.000000           Time.isDST() = 1
    Time.getDSTOffset() = 1.000000 hours
    The current time in this time zone is: Fri Oct  6 05:56:28 2017   AEDT
------------------------------------------------ SETTINGS AFTER DST TRANSITION ---
Australia/Tasmania
         Current Offset: 10.000000 hours, Standard Offset: 10.000000 hours
        No DST transitions pending.
            EEPROM Data Updated
Device Settings: - - - - - - - - - - - - - - - - - -
            Time.zone() = 10.000000           Time.isDST() = 0
    Time.getDSTOffset() = 0.000000 hours
    The current time in this time zone is: Fri Oct  6 04:56:28 2017   AEST

-----------------------------------------------------------------------------------
Press 'q' to query EEPROM, 'c' to change time zone, or 't' to transition now:

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< END SERIAL MONITOR CAPTURE 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
