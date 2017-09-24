#ifndef __TZLIB_H_
#define __TZLIB_H_
#include "application.h"
#include <HttpClient.h>


// This struct is stored in EEPROM
typedef struct {
    char version[10];           // if the TimeZoneInfo struct needs to change, this field will tell us how to interpret the data stored in EEPROM
    char zoneId[65];            // the IANA defined time zone ID assigned to this device
    float stdOffset;            // the standard zone offset for the above time zone
    float currentOffset;        // the current offset for the above time zone
    time_t transitionTime;      // the EpochSecond when the current offset is scheduled to change
    float transitionOffset;     // the offset that this timezone will adopt when the above EpochSecond arrives
} tzInfo_t;


// Methods --------------------------------------------------------

    int tzSetup(char* = NULL); // <-- Note: tzSetup accepts a "time zone ID" as an optional arguement, when not provided, the arguement is NULL.
    void tzLoop();
    tzInfo_t tzGetInfo();
    char* tzGetSetupReturnMsg();
    void tzSetDefaultZoneId(char*);
    void tzSetTransitionTime(time_t);
    int tzWipeEEPROM();
    void tzSetEepromLocation(int);
    void tzSetHttpHost(char*);
    void tzSetHttpPath(char*);

#endif