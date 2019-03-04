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

#include <string>
#include <iostream>
#include <cstring>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <json-c/json.h>
#include "configuration.h"
#include "afbclient.h"
#include "mqttclient.h"
#include "network.h"
#include "event.h"

#define CONFIGURATION_FILE	"telematics-recorder.conf"
#define DEVICE_UUID		"e4bbc0a8-f435-4326-9769-d4a2c9f3c18d"

static std::deque<event_data*> g_event_queue;
static std::mutex g_event_queue_mutex;
static bool g_event_queue_ready = false;
static std::condition_variable g_event_queue_cv;

static Configuration g_config(CONFIGURATION_FILE, DEVICE_UUID);

void diagnostic_message_cb(json_object *data)
{
	if(!data)
		return;

	if(g_config.getLogLevel() > 2) {
		std::cerr << __FUNCTION__ << ": data = " <<		\
			json_object_to_json_string_ext(data, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << \
			std::endl;
	}

	struct json_object *j_name;
	if(!json_object_object_get_ex(data, "name", &j_name))
		return;
	struct json_object *j_value;
	if(!json_object_object_get_ex(data, "value", &j_value))
		return;
	struct json_object *j_timestamp;
	if(!json_object_object_get_ex(data, "timestamp", &j_timestamp))
		return;
	uint64_t timestamp = json_object_get_int64(j_timestamp);

	std::string name(json_object_get_string(j_name));
	int32_t value = json_object_get_int(j_value);
	if(name == "diagnostic_messages.vehicle.speed" ||
	   name == "diagnostic_messages.engine.speed") {
		if(g_config.getLogLevel() > 1) {
			std::cerr << __FUNCTION__ << ": " << name << \
			  ", value = " << value << \
			  ", timestamp = " << timestamp << \
			  std::endl;
		}

		name.erase(0, 20);
		event_data* event = new event_data{ name, value, timestamp };
		{
			std::lock_guard<std::mutex> lk(g_event_queue_mutex);
			g_event_queue.push_back(event);
			g_event_queue_ready = true;
		}
		g_event_queue_cv.notify_one();
	}
}

int main(int argc, char *argv[])
{
	int port = 0;
	std::string token;

	try {
		port = std::stol(argv[1]);
		token = argv[2];
	} catch (const std::invalid_argument& e) {
		std::cerr << "Invalid argument" << std::endl;
		exit(1);
	} catch (const std::out_of_range& e) {
		std::cerr << "Port out of range" << std::endl;
		exit(1);
	}

	AfbClient afbclient(port, token);

	if(g_config.isCellularEnabled()) {
		// Wait for modem to appear, and enable it if not already
		enable_modem(g_config, afbclient);
	}

	std::string client_id = g_config.getMqttClientId();
	if(client_id.empty())
		client_id = std::string("AGL-telematics-recorder") + DEVICE_UUID;
	MqttClient mqttclient(client_id,
			      g_config.getMqttBroker(),
			      g_config.getMqttPort(),
			      g_config.getMqttKeepalive(),
			      g_config.getMqttUsername(),
			      g_config.getMqttPassword());

	afbclient.subscribe(std::string("low-can"),
			    std::string("diagnostic_messages.vehicle.speed"),
			    std::string("low-can/diagnostic_messages"),
			    diagnostic_message_cb);
	afbclient.subscribe(std::string("low-can"),
			    std::string("diagnostic_messages.engine.speed"),
			    std::string("low-can/diagnostic_messages"),
			    diagnostic_message_cb);

	std::deque<event_data*> event_queue;
	while(true) {
		// Wait until event callback sends data
		std::unique_lock<std::mutex> lk(g_event_queue_mutex);
		g_event_queue_cv.wait(lk, []{ return g_event_queue_ready; });
		if(!g_event_queue.empty()) {
			// copy out the events
			event_queue = g_event_queue;
			g_event_queue.clear();
		}
		g_event_queue_ready = false;
		lk.unlock();

		for(event_data *event : event_queue) {
			if(!event)
				continue;

			if(g_config.getLogLevel() > 0) {
				std::cerr << __FUNCTION__ << ": " << \
					event->name << ", value = " << event->value << \
					", timestamp = " << event->timestamp << \
					std::endl;
			}

			process_event(g_config, afbclient, mqttclient, event);

			delete event;
		}
		// Clear out processed events
		event_queue.clear();
	}
	return 0;
}
