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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

class Configuration
{
public:
	Configuration(const std::string &file, const std::string &device_uuid = "");
	~Configuration() {};

	uint32_t getLogLevel() { return m_log_level; }
	bool isCellularEnabled() { return m_cellular_enabled; }
	bool isGpsEnabled() { return m_gps_enabled; }
	bool getCheckOnline() { return m_check_online; }
	std::string getDeviceUUID() { return std::string(m_device_uuid); }

	std::string getMqttClientId() { return std::string(m_mqtt_client_id); }
	std::string getMqttBroker() { return std::string(m_mqtt_broker); }
	uint32_t getMqttPort() { return m_mqtt_port; }
	uint32_t getMqttKeepalive() { return m_mqtt_keepalive; }
	uint32_t getMqttQos() { return m_mqtt_qos; }
	bool getMqttRetain() { return m_mqtt_retain; }
	std::string getMqttUsername() { return std::string(m_mqtt_username); }
	std::string getMqttPassword() { return std::string(m_mqtt_password); }

	uint32_t getUpdatePeriod() { return m_update_period; }

private:
	void read();

	std::string m_filename;

	// General
	uint32_t m_log_level = 0;
	bool m_cellular_enabled = false;
	bool m_gps_enabled = false;
	bool m_check_online = false;
	std::string m_device_uuid;

	// MQTT broker
	std::string m_mqtt_client_id = "";
	std::string m_mqtt_broker = "iot.eclipse.org";
	uint32_t m_mqtt_port = 1883;
	uint32_t m_mqtt_keepalive = 60;
	uint32_t m_mqtt_qos = 0;
	bool m_mqtt_retain = true;
	std::string m_mqtt_username = "";
	std::string m_mqtt_password = "";

	// Event
	uint32_t m_update_period = 10;
};

#endif // CONFIGURATION_H
