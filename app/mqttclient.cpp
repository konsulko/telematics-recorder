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

#include "mqttclient.h"
#include <iostream>

#undef DEBUG

#ifdef DEBUG
static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
	std::cerr << " MQTT Connected, rc = " << rc << std::endl;
}

static void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
	std::cerr << " MQTT Disconnected, rc = " << rc << std::endl;
}
#endif

MqttClient::MqttClient(const std::string &id, const std::string &host, const int port, const int keepalive, const std::string &username, const std::string &password)
{
	mosquitto_lib_init();
	m_mosq = mosquitto_new(id.c_str(), true, NULL);

#ifdef DEBUG
	mosquitto_connect_callback_set(m_mosq, on_connect);
	mosquitto_disconnect_callback_set(m_mosq, on_disconnect);
#endif

	if(username.length())
		mosquitto_username_pw_set(m_mosq, username.c_str(), password.c_str());

	if(mosquitto_connect_async(m_mosq, host.c_str(), port, keepalive)) {
		std::cerr << __FUNCTION__ << ": Unable to connect to " << host << std::endl;
	}

	int loop = mosquitto_loop_start(m_mosq);
	if(loop != MOSQ_ERR_SUCCESS){
		std::cerr << __FUNCTION__ << ": Unable to start loop, error = " << loop << std::endl;
	}
}

MqttClient::~MqttClient(void)
{
	mosquitto_disconnect(m_mosq);
	mosquitto_loop_stop(m_mosq, true);
	mosquitto_destroy(m_mosq);
	mosquitto_lib_cleanup();
}

int MqttClient::publish(const std::string &topic, const std::string &msg, const int qos, const bool retain)
{
	return mosquitto_publish(m_mosq, NULL, topic.c_str(), msg.length(), msg.c_str(), qos, retain);
}
