/*
 * Copyright (C) 2019 Konsulko Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gps.h"
#include <string>
#include <iostream>
#include <json-c/json.h>

bool get_location(Configuration &config, AfbClient &afbclient, location_data *location)
{
	bool rc;
	struct json_object *j_resp = NULL;
	std::string status;

	if(!location)
		return false;

	if(afbclient.call_sync(std::string("gps"), std::string("location"), NULL, &j_resp) < 0)
		return false;

	if(config.getLogLevel() > 1) {
		std::cerr << __FUNCTION__ << ": j_resp = " <<		\
			json_object_to_json_string_ext(j_resp, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << \
			std::endl;
	}

	// Check status
	rc = false;
	struct json_object *j_request;
	if(!json_object_object_get_ex(j_resp, "request", &j_request))
		goto location_error;

	struct json_object *j_status;
	if(!json_object_object_get_ex(j_request, "status", &j_status))
		goto location_error;

	status = json_object_get_string(j_status);
	if(status == "failed")
		goto location_error;

	// Get location data
	struct json_object *j_response;
	if(!json_object_object_get_ex(j_resp, "response", &j_response))
		goto location_error;

	struct json_object *j_latitude;
	if(!json_object_object_get_ex(j_response, "latitude", &j_latitude))
		goto location_error;

	struct json_object *j_longitude;
	if(!json_object_object_get_ex(j_response, "longitude", &j_longitude))
		goto location_error;

	struct json_object *j_speed;
	if(!json_object_object_get_ex(j_response, "speed", &j_speed))
		goto location_error;

	struct json_object *j_track;
	if(!json_object_object_get_ex(j_response, "track", &j_track))
		goto location_error;

	struct json_object *j_timestamp;
	if(!json_object_object_get_ex(j_response, "timestamp", &j_timestamp))
		goto location_error;

	location->latitude = json_object_get_double(j_latitude);
	location->longitude = json_object_get_double(j_longitude);
	location->speed = json_object_get_int(j_speed);
	location->track = json_object_get_int(j_track);
	location->timestamp = json_object_get_string(j_timestamp);
	rc = true;

location_error:
	json_object_put(j_resp);
	return rc;
}
