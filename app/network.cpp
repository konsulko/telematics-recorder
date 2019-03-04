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

#include "network.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <json-c/json.h>

int enable_modem(Configuration &config, AfbClient &afbclient)
{
	int rc;
	struct json_object *j_resp = NULL;

	bool cellular_enabled = false;
	bool cellular_found = false;
	while(!cellular_found) {
		// Check current state
		rc = afbclient.call_sync(std::string("network-manager"), std::string("technologies"), NULL, &j_resp);
		if(rc < 0 || !j_resp)
			continue;
		if(config.getLogLevel() > 1) {
			std::cerr << __FUNCTION__ << ": j_resp = " <<	\
				json_object_to_json_string_ext(j_resp, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << \
				std::endl;
		}

		struct json_object *j_response;
		if(!json_object_object_get_ex(j_resp, "response", &j_response))
			continue;

		struct json_object *j_values;
		if(!json_object_object_get_ex(j_response, "values", &j_values))
			continue;

		for(int i = 0; i < json_object_array_length(j_values); i++) {
			struct json_object *j_value = json_object_array_get_idx(j_values, i);
			if(!j_value)
				break;

			struct json_object *j_technology;
			if(!json_object_object_get_ex(j_value, "technology", &j_technology))
				break;

			std::string technology(json_object_get_string(j_technology));
			if(technology == "cellular") {
				struct json_object *j_properties;
				if(!json_object_object_get_ex(j_value, "properties", &j_properties))
					break;

				struct json_object *j_powered;
				if(!json_object_object_get_ex(j_properties, "powered", &j_powered))
					break;

				if(json_object_get_boolean(j_powered)) {
					if(config.getLogLevel() > 0) {
						std::cerr << __FUNCTION__ << ": cellular enabled!" << std::endl;
						cellular_enabled = true;
					}
				}

				std::cerr << __FUNCTION__ << ": cellular found!" << std::endl;
				cellular_found = true;
				rc = 0;
				break;
			}
			
		}
		json_object_put(j_resp);

		sleep(1);
	}

	if(!cellular_enabled) {
		if(config.getLogLevel() > 0) {
			std::cerr << __FUNCTION__ << ": enabling cellular" << std::endl;
		}
		struct json_object *j_obj = json_object_new_object();
		struct json_object *j_val = json_object_new_string("cellular");
		json_object_object_add(j_obj, "technology", j_val);
		rc = afbclient.call_sync(std::string("network-manager"), std::string("enable_technology"), j_obj);
	}
	return rc;
}

bool check_online(AfbClient &afbclient)
{
	int rc;
	struct json_object *j_resp = NULL;

	// Check current state
	rc = afbclient.call_sync(std::string("network-manager"), std::string("state"), NULL, &j_resp);
	if(rc < 0 || !j_resp)
		return false;

	struct json_object *j_response;
	if(!json_object_object_get_ex(j_resp, "response", &j_response))
		return false;

	bool online = false;
	std::string response(json_object_get_string(j_response));
	if(response == "online")
		online = true;
	json_object_put(j_resp);
	return online;
}
