#include "application.h"
#include "tzLib.h"
#include "spark_wiring_string.h"
#include "spark_wiring_tcpclient.h"
#include "spark_wiring_usbserial.h"

/*      Library: tzLib
        Module: Http.cpp contains the method that this library needs to perform
        an HTTP POST query to obtain time zone data from an HTTP server. 
        
        The Http class is defined and instantiated in tzLib.
        
        Methods are called using the http object. Format: http.method();

        Note:   Class methods are tailored specifically to meet tzLib
                requirements as efficiently as possible. Since this project
                encompasses both the library and the HTTP server code, a 
                bare-bones approach can be used. 
*/
// ----------------------------------------------------------- Class Constructor
Http::Http() {
    error = false;
    memset(&buffer[0], 0, sizeof(buffer));  // clears the response buffer
    bufferIndex = 0;
};

// ----------------------------------------------------------- getJson()
int Http::getJson(char* hostName, int hostPort, char* hostPath, char* jsonStr, int jsonSize, char* statusMsg, int statusMsgSize) {
        
    client.connect(hostName, hostPort);
    if (client.connected()) {
        client.print("POST ");
        client.print(hostPath);
        client.print(" HTTP/1.0\r\n");
    	client.println("Connection: close");
    	client.print("HOST: ");
    	client.println(hostName);
    	client.print("Content-Length: ");
    	client.println(String(strlen(jsonStr)));
    	client.println("Content-Type: application/json");
    	client.println("Accept: application/json");
        client.println();
        client.flush();
        client.println(jsonStr);
    } else {
        error = true;
        strncpy(statusMsg, "ERROR - Unable to connect to Server/Port", statusMsgSize);
        client.stop();
    }

    #ifdef LOGGING
        Serial.println("\r\nHttp>\t---------- Start Http.getJson() ----------");
        Serial.print("Http>\thostName = ");
    	Serial.println(String(hostName));
        Serial.print("Http>\thostPort = ");
    	Serial.println(hostPort);
        Serial.print("Http>\thostPath = ");
        Serial.println(String(hostPath));
        Serial.print("Http>\tjsonStr  = ");
        Serial.println(String(jsonStr));
        Serial.print("Http>\tjsonSize = ");
        Serial.println(jsonSize);
    	if (client.connected()) {
    		Serial.println("Http>\tClient is Connected to the HTTP server ...");
    		Serial.println("Http>\tHTTP POST Request has been Sent ...");
    	} else {
    		Serial.println(statusMsg);
        }
    #endif
    if (error) {
	    return -1;
    }
       
    // ------------------------------------------------------------------------------ Buffer the  Response
    bufferIndex = 0;
    startMillis = millis();
    lastReadMillis = millis();

    
    while (client.connected() && !error) {
        while (client.available() && !error) {
                char c = client.read();
           lastReadMillis = millis();
            if (bufferIndex == sizeof(buffer)-1) {
                // buffer overflow
                error = true;
                strncpy(statusMsg, "ERROR - Response Buffer Overflow", statusMsgSize);
            } else {
                buffer[bufferIndex] = c;
                bufferIndex++;
            }
        }
        buffer[bufferIndex] = '\0';
        if ((millis() - lastReadMillis) > 5000) {
            // timeout after 5 seconds of inactivity
            error = true;
            strncpy(statusMsg, "ERROR - Timeout waiting for server to respond", statusMsgSize);
 
        } else if ( !error) {
            delay(200);
        }
    }
    client.stop();

    #ifdef LOGGING
        Serial.print("\r\nHttp>\tResponse Time (ms): ");
        Serial.println(millis() - startMillis);
        Serial.println("---------------------------------------------------- Start of HTTP Response");
        Serial.println(buffer);
        Serial.println("---------------------------------------------------- End of HTTP Response");
        if(error) {
            Serial.println(statusMsg);
        } else {
            Serial.println("\r\nHttp>\tNo Client Processing Errors Detected ...");
        }
    #endif
	if (error) {
	    return -1;
	}	
    
    // --------------------------------- Parse statusCode and tzLib JSON from buffer
    // This is an example of what the JSON should look like --- 
    // {"sOffset":-6,"cOffset":-5,"cAbbr":"CDT","tTime":1509865200,"tOffset":-6,"tAbbr":"CST"}

    
    char* cp;
    cp = strstr(buffer, " ");
    int statusCode = atoi(cp+1);
    if (statusCode == 200) {
        cp = strstr(buffer, "{\"sOffset\":");
	    if (cp == NULL) {
	        jsonStr[0] = '\0';
	        error = true;
	        strncpy(statusMsg, "ERROR - JSON not found in buffer", statusMsgSize);
	    } else {
            char* lcp = (char*)memccpy(jsonStr, cp, '}', jsonSize-1);   // find the closing bracket
            if (lcp == NULL) {
	        jsonStr[0] = '\0';
	        error = true;
	        strncpy(statusMsg, "ERROR - JSON was truncated in buffer", statusMsgSize);
            } else {
                memset(lcp,'\0', 1);                     // add a '\0' to the end of the JSON string
	        }
	    }
    }
    #ifdef LOGGING
        Serial.print("Http>\tHTTP Status Code: ");
        Serial.println(String(statusCode));
        Serial.print("Http>\tJSON: ");
        Serial.println(String(jsonStr));
        if (error) {
            Serial.print("Http>\t");
            Serial.println(statusMsg);
        }
     #endif

	if (error) {
	    return -1;
	} else {
	    switch (statusCode) {
	        case 200 :
        	    strncpy(statusMsg, "HTTP Processing Completed Normally", statusMsgSize);
        	    break;
        	case 404 :
        	    strncpy(statusMsg, "HTTP 404 ERROR - Invalid Host Path", statusMsgSize);
        	    break;
        	case 422 :
        	    strncpy(statusMsg, "HTTP 422 ERROR - Invalid Time Zone ID", statusMsgSize);
        	    break;
        	default:
        	    strncpy(statusMsg, "HTTP ", statusMsgSize);
        	    strncat(statusMsg, String(statusCode), statusMsgSize);
        	    strncat(statusMsg, " ERROR Returned", statusMsgSize);
	    }
	    #ifdef LOGGING
	        Serial.print("Http>\t");
	        Serial.println(statusMsg);
	    #endif
	    return statusCode;
	}

}

