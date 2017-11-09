#include "tzLib.h"

/*      Library: tzLib
        Module: Json.cpp contains json encoding and decoding methods for this library

        Methods are called using the json object. Format: json.method();

        Note:   Class methods are tailored specifically to meet tzLib
                requirements as efficiently as possible. Since all of the JSON
                consumed by this project are created within the project, a 
                simple, bare-bones approach can be used. 
*/

// ----------------------------------------------------------- Class Constructor
Json::Json(void) {
}

// ----------------------------------------------------------- Encoding Methods

/*  Encode the query that will be sent to the http server
    Arguements:
        jsonStr Points to the character array where we write the JSON
        tzID    Points to the character array that contains a time zone ID
*/
void Json::encodeQuery(char* jsonStr, char* tzId) {
    strcpy (jsonStr, "{\"timeZoneID\":\"");
    strcat (jsonStr, tzId);
    strcat (jsonStr, "\"}");
    
    #ifdef LOGGING
    Serial.print("json>\tEncoded: ");
    Serial.println(jsonStr);
    #endif
    
    return;
}

// ----------------------------------------------------------- Decoding Methods

/*  Extract a floating point number
    Arguements:
        n       Points to the 'float' where the extracted number will be written
        jsonStr Points to the character array that contains the JSON string
                from which we extract the value.
        name    Specifies the JSON name associated with the JSON value.
*/

int Json::get(float& n, char* jsonStr, char* name) {
    char str[33] = "\"";
    strncat(str,name,sizeof(str));
    strncat(str,"\":",sizeof(str));
    char* pch = strstr(jsonStr,str);
    if (pch != NULL) { 
        n = atof(pch+strlen(str));
        
        #ifdef LOGGING
            Serial.print("json>\tParsed: ");
            Serial.print(String(name));
            Serial.print(" = ");
            Serial.println(String(n));
        #endif

        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

/*  Extract a time_t time stamp
    Arguements:
        n       Points to the 'time_t' where the extracted time stamp will be written
        jsonStr Points to the character array that contains the JSON string
                which we extract the value from.
        name    Specifies the JSON name associated with the JSON value.
*/
int Json::get(time_t& n, char* jsonStr, char* name) {
    char str[33] = "\"";
    strncat(str,name,sizeof(str));
    strncat(str,"\":",sizeof(str));
    char* pch = strstr(jsonStr,str);
    if (pch != NULL) { 
        n = atol(pch+strlen(str));
        
        #ifdef LOGGING
            Serial.print("json>\tParsed: ");
            Serial.print(String(name));
            Serial.print(" = ");
            Serial.println(String(n));
        #endif

        
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

/*  Extract a small character array
    Arguements:  
        ca      Points to a character array where the extracted JSON value 
                will be written.
        caSize  Specifies sizeof(ca).
        jsonStr Points to the character array that contains the JSON string
                which we extract the value from.
        name    Specifies the JSON name associated with the JSON value.
*/    
int Json::get(char* ca, int caSize, char* jsonStr, char* name) {
    char str[33] = "\"";
    strncat(str,name,sizeof(str));
    strncat(str,"\":\"",sizeof(str));
    char* pch = strstr(jsonStr,str);
    if (pch != NULL) {
        char* lch = (char*)memccpy(ca, pch + strlen(str), '\"', caSize);
        if (lch != NULL) { 
            // memccpy() copies the value's closing quote. 
            // We try to locate the closing quote with memchr().
            lch = (char*)memchr(ca, '\"', caSize);
            if (lch != NULL) {
                // The closing quote was located.
                // We convert the closing quote to a '\0' and exit
                memset(lch, '\0', 1);
                
                    #ifdef LOGGING
                        Serial.print("json>\tParsed: ");
                        Serial.print(String(name));
                        Serial.print(" = ");
                        Serial.println(String(ca));
                     #endif

                 return EXIT_SUCCESS;
            }
        }
        /*  The closing quote was not located, so ...
                the JSON value was probably too long, but it is 
                possible that the JSON was somehow corrupted.
            We change the last character of the array to a '\0'.
            The value is therefore truncated to the the size of the array.
            EXIT_FAILURE is returned so the caller can determine if 
                the value should be used. 
        */
        memset(ca + (caSize-1), '\0', 1);
        
                    #ifdef LOGGING
                        Serial.print("json>\tParsed: ");
                        Serial.print(String(name));
                        Serial.print(" = ");
                        Serial.println(String(ca));
                     #endif

        return EXIT_FAILURE;
    }
}



















