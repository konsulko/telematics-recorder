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

#include "event.h"
#include "gps.h"
#include "network.h"
#include <string>
#include <iostream>
#include <json-c/json.h>
#include <time.h>

static uint64_t last_vehicle_speed_update_usecs;
static uint64_t last_engine_speed_update_usecs;

void process_event(Configuration &config, AfbClient &afbclient, MqttClient &mqttclient, event_data *event)
{
	std::string topic("agl-telematics-demo/");
	topic += event->name;
	struct json_object *j_obj = json_object_new_object();
	struct json_object *j_device = json_object_new_string(config.getDeviceUUID().c_str());
	json_object_object_add(j_obj, "device", j_device);
	struct json_object *j_val = json_object_new_int(event->value);
	json_object_object_add(j_obj, "value", j_val);
	struct json_object *j_ts = json_object_new_int64(event->timestamp);
	json_object_object_add(j_obj, "timestamp", j_ts);

	location_data location;
	struct json_object *j_location = json_object_new_object();
	if(config.isGpsEnabled() && get_location(config, afbclient, &location)) {
		struct json_object *j_latitude = json_object_new_double(location.latitude);
		json_object_object_add(j_location, "latitude", j_latitude);
		struct json_object *j_longitude = json_object_new_double(location.longitude);
		json_object_object_add(j_location, "longitude", j_longitude);
		struct json_object *j_speed = json_object_new_int(location.speed);
		json_object_object_add(j_location, "speed", j_speed);
		struct json_object *j_track = json_object_new_int(location.track);
		json_object_object_add(j_location, "track", j_track);
		struct json_object *j_timestamp = json_object_new_string(location.timestamp.c_str());
		json_object_object_add(j_location, "timestamp", j_timestamp);
	}
	json_object_object_add(j_obj, "location", j_location);

	std::string msg(json_object_to_json_string_ext(j_obj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
	if(config.getLogLevel() > 1) {
		std::cerr << __FUNCTION__ << ": topic = " << topic << ", msg = " << msg << std::endl;
	}

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	uint64_t now_usecs = now.tv_sec * 1000000 + now.tv_nsec / 1000;
	uint64_t *past_usecs = NULL;
	if(event->name == "vehicle.speed")
		past_usecs = &last_vehicle_speed_update_usecs;
	else if(event->name == "engine.speed")
		past_usecs = &last_engine_speed_update_usecs;
	if(!config.getUpdatePeriod() ||
	   past_usecs &&
	   (!*past_usecs ||
	    ((now_usecs - *past_usecs) > (config.getUpdatePeriod() * 1000000)))) {
		int rc = -1;
		if(config.getCheckOnline() && check_online(afbclient)) {
			goto skip_update;
		}
		rc = mqttclient.publish(topic, msg, config.getMqttQos(), config.getMqttRetain());
		if(rc != MOSQ_ERR_SUCCESS) {
			std::cerr << __FUNCTION__ << ": MQTT publish failed, rc = " << rc << std::endl;
		} else if(config.getLogLevel() > 0) {
			std::cerr << __FUNCTION__ << ": MQTT publish succeeded" << std::endl;
		}
skip_update:
		*past_usecs = now_usecs;
	}
}
