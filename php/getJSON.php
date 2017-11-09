
    <?php
		/* 	Module: GetTzLibJSON.php
		 	Version: 1.0.2a
		  	Input: 	JSON object {"timeZoneID":"<timezoneid>"}, where <timezoneid> is
		 		- 	a valid IANA time zone ID (aka Olson Name)
				-	provided in the request body
		 	Output: JSON object $JSON defined and initialized below.
			HTTP Errors:
				-	(204) thrown when the required input variable 'timeZoneID' is not found
				- 	(404) thrown when the 'timeZoneID' is not found in the tzInfo database
		*/
		
        ini_set('display_errors', '1');
        ini_set('error_reporting', E_ALL);
		header('Content-Type: application/json');
		
		// Initialize the JSON output variables
		$JSON = new StdClass;		// <-- JSON Output Object
		$JSON->sOffset = 0.0;		// <-- time zone's standard offset (in hours)
		$JSON->cOffset = 0.0;		// <-- time zone's current offset in (in hours)
		$JSON->cAbbr = "";			// <-- time zone's current abbreviation (text)
		$JSON->tTime = 0.0;			// <-- time (in epoch seconds) when the current offset will change
		$JSON->tOffset = 0.0;		// <-- the time zone's current offset after the change occurs (in hours)
		$JSON->tAbbr = "";			// <-- the timezone's current abbreviation after the change occurs (text)
		
		// Process Request Input
		$_POST = json_decode(file_get_contents('php://input'), true);
        if ( !isset($_POST["timeZoneID"])) {
			http_response_code(204);
			echo "timeZoneID variable was not received by server";
			return;
		}
		$timeZoneID = $_POST["timeZoneID"];
		// Create DateTimeZone object for the selected timezone, and exit if the timeZoneID is invalid
		try {
			$dateTimeZoneTZ = new DateTimeZone($timeZoneID);
		} catch(Exception $e) {
			echo $e->getMessage(), "<br/>";
			http_response_code(422);
			return;
		}
		
		// Extract data from the selected timezone's DateTimeZone object 
		$transitions = $dateTimeZoneTZ->getTransitions(time(), strtotime('+1 year'));
		for ($i = 0; $i < count($transitions); $i++) {
			if ( !$transitions[$i]['isdst']) { 	// 	<-- isdst = false identifies the the offset as the standard offset.
			// PHP provides offsets in integer seconds, which we convert to floating point hours.
				$JSON->sOffset = floatval($transitions[$i]['offset'])/3600.0;
			}
			if ($i == 0) {			// <-- First transition represents the current transition period
				$JSON->cOffset = floatval($transitions[$i]['offset'])/3600.0;
				$JSON->cAbbr = $transitions[$i]['abbr'];
			} else if ($i == 1) {	// <-- Second transition represents the next transition period
				$JSON->tTime = $transitions[$i]['ts'];
				$JSON->tOffset = floatval($transitions[$i]['offset'])/3600.0;
				$JSON->tAbbr = $transitions[$i]['abbr'];
			}
		}
		echo json_encode($JSON);
    ?>
