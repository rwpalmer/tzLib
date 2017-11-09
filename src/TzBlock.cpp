#include "tzLib.h"

/*      Library: tzLib
        Module: TzBlock.cpp contains methods that construct and manipulate TzBlock objects.

        The TzBlock class was created to define the memory block that tzLib reads and writes
        to the devices EEPROM memory. Multiple instances of this class are required for for
        processing. 

*/

// ---------------------------------------------------------------------------- constructor
TzBlock::TzBlock(void) {
    strncpy(signature, TZ_SIGNATURE, sizeof(signature));
	strcpy(id,"");
	stdOffset = 0;
	curOffset = 0;
	strcpy(curAbbr,"");
	tranTime = 0;
	tranOffset = 0;
	strcpy(tranAbbr,"");
	std::fill_n(futureUse,sizeof(futureUse),0xFF);
}

// ---------------------------------------------------------------------------- equals()
// compares two TzBlock objects.
// returns 'true' if the blocks contain the same data.
bool TzBlock::equals(TzBlock tz) {
    bool equal = false;
    if ((strcmp(signature, tz.signature) == 0) &&
        (strcmp(id, tz.id) == 0) &&
        (stdOffset == tz.stdOffset) &&
        (curOffset == tz.curOffset) &&
        (strcmp(curAbbr, tz.curAbbr) == 0) &&
        (tranTime == tz.tranTime) &&
        (tranOffset == tz.tranOffset) &&
        (strcmp(tranAbbr, tz.tranAbbr) == 0)) {
        equal = true;
    }
    #ifdef LOGGING
        Serial.print("TzBlock>\tTzBlocks were ");
        Serial.println(equal? "equal":"NOT equal");
    #endif
            
    return equal;
}

// ---------------------------------------------------------------------------- applyJson()
// Updates a TzBlock based on the JSON received from the HTTP server
int TzBlock::applyJson(char* jsonStr) {

    //This is an example of what the JSON should look like --- 
    //  {"sOffset":-6,"cOffset":-5,"cAbbr":"CDT","tTime":1509865200,"tOffset":-6,"tAbbr":"CST"}
    Json json;
    char* pch;
    char* qch;
    bool parsingError = false;
    if (json.get(stdOffset, (char*)jsonStr, (char*)"sOffset") == EXIT_FAILURE) {
        parsingError = true;
    }
    if (json.get(curOffset, (char*)jsonStr, (char*)"cOffset") == EXIT_FAILURE) {
        parsingError = true;
    }
    json.get(curAbbr, sizeof(curAbbr), (char*) jsonStr, (char*) "cAbbr");
    json.get(tranTime, (char*)jsonStr, (char*)"tTime");
    json.get(tranOffset, (char*)jsonStr, (char*)"tOffset");
    json.get(tranAbbr, sizeof(tranAbbr), (char*) jsonStr, (char*) "tAbbr");
    
    #ifdef LOGGING
        Serial.print("TzBlock>\tJSON Parsing ");
        Serial.println(parsingError? "FAILED":"Successful");
    #endif
    
    if (parsingError) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}

    
// -------------------------------------------------------------------- log()
// Displays the contents of a TzBlock when debugging
void TzBlock::log(char* name) {
    # ifdef LOGGING
		Serial.print("TzBlock>\t-------- ");
		Serial.print(name);
		Serial.println(" --------");
		Serial.print("TzBlock>\tid         = ");
		Serial.println(String(id));
		Serial.print("TzBlock>\tstdOffset  = ");
		Serial.println(String(stdOffset));
		Serial.print("TzBlock>\tcurOffset  = ");
		Serial.println(String(curOffset));
		Serial.print("TzBlock>\tcurAbbr    = ");
		Serial.println(String(curAbbr));
		Serial.print("TzBlock>\ttranTime    = ");
		Serial.println(String(tranTime));
		Serial.print("TzBlock>\ttranOffset  = ");
		Serial.println(String(tranOffset));
		Serial.print("TzBlock>\ttranAbbr    = ");
		Serial.println(String(tranAbbr));
	#endif
}


















