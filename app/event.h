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

#ifndef EVENT_H
#define EVENT_H

#include "configuration.h"
#include "afbclient.h"
#include "mqttclient.h"

struct event_data {
	std::string name;
	int32_t value;
	uint64_t timestamp;
};

void process_event(Configuration &config, AfbClient &afbclient, MqttClient &mqttclient, event_data *event);

#endif // EVENT_H
