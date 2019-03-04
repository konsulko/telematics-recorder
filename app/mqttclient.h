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

#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <string>
#include <mosquitto.h>
#include <json-c/json.h>

class MqttClient
{
public:
	MqttClient(const std::string &id, const std::string &host, const int port, const int keepalive = 60, const std::string &username = "", const std::string &password = "");
	  ~MqttClient();

	int publish(const std::string &topic, const std::string &msg, const int qos, const bool retain);

private:
	mosquitto *m_mosq;
};

#endif // MQTTCLIENT_H
