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

#include "configuration.h"
#include <glib.h>
#include <strings.h>

Configuration::Configuration(const std::string &filename, const std::string &device_uuid):
	m_filename(filename),
	m_device_uuid(device_uuid)
{
	read();
}

void Configuration::read(void)
{
	GKeyFile* conf_file;
	GError *error = NULL;
	char *value_str;
	int n;

	// Load settings from configuration file if it exists
	conf_file = g_key_file_new();
	if(!conf_file) {
		return;
	}

	if(g_key_file_load_from_dirs(conf_file,
				     m_filename.c_str(),
				     (const gchar**) g_get_system_config_dirs(),
				     NULL,
				     G_KEY_FILE_KEEP_COMMENTS,
				     NULL) != TRUE) {
		g_key_file_free(conf_file);
		return;
	}

	//
	// General
	//

	// Set log level if it is specified
	error = NULL;
	n = g_key_file_get_integer(conf_file, "General", "log_level", &error);
	if(!error && n > 0) {
		m_log_level = n;
	}

	value_str = g_key_file_get_string(conf_file, "General", "cellular_enabled", NULL);
	if(value_str) {
		if(!strcasecmp(value_str, "true")) {
			m_cellular_enabled = true;
		} else if(!strcasecmp(value_str, "false")) {
			m_cellular_enabled = false;
		}
	}

	value_str = g_key_file_get_string(conf_file, "General", "gps_enabled", NULL);
	if(value_str) {
		if(!strcasecmp(value_str, "true")) {
			m_gps_enabled = true;
		} else if(!strcasecmp(value_str, "false")) {
			m_gps_enabled = false;
		}
	}

	value_str = g_key_file_get_string(conf_file, "General", "check_online", NULL);
	if(value_str) {
		if(!strcasecmp(value_str, "true")) {
			m_check_online = true;
		} else if(!strcasecmp(value_str, "false")) {
			m_check_online = false;
		}
	}

	value_str = g_key_file_get_string(conf_file, "General", "device_uuid", NULL);
	if(value_str) {
		m_device_uuid = value_str;
	}

	//
	// MQTT
	//

	value_str = g_key_file_get_string(conf_file, "MQTT", "broker", NULL);
	if(value_str) {
		m_mqtt_broker = value_str;
	}

	error = NULL;
	n = g_key_file_get_integer(conf_file, "MQTT", "port", &error);
	if(!error && n > 0) {
		m_mqtt_port = n;
	}

	error = NULL;
	n = g_key_file_get_integer(conf_file, "MQTT", "keepalive", &error);
	if(!error && n >= 0) {
		m_mqtt_keepalive = n;
	}

	error = NULL;
	n = g_key_file_get_integer(conf_file, "MQTT", "qos", &error);
	if(!error && n >= 0 && n < 3) {
		m_mqtt_qos = n;
	}

	value_str = g_key_file_get_string(conf_file, "MQTT", "retain", NULL);
	if(value_str) {
		if(!strcasecmp(value_str, "true")) {
			m_mqtt_retain = true;
		} else if(!strcasecmp(value_str, "false")) {
			m_mqtt_retain = false;
		}
	}

	value_str = g_key_file_get_string(conf_file, "MQTT", "username", NULL);
	if(value_str) {
		m_mqtt_username = value_str;
	}

	value_str = g_key_file_get_string(conf_file, "MQTT", "password", NULL);
	if(value_str) {
		m_mqtt_password = value_str;
	}

	//
	// Event
	//

	error = NULL;
	n = g_key_file_get_integer(conf_file, "Event", "update_period", &error);
	if(!error && n >= 0) {
		m_update_period = n;
	}

	g_key_file_free(conf_file);
}
