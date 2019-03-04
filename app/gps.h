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

#ifndef GPS_H
#define GPS_H

#include "configuration.h"
#include "afbclient.h"

struct location_data {
	double latitude;
	double longitude;
	int32_t speed;
	int32_t track;
	std::string timestamp;
};

bool get_location(Configuration &config, AfbClient &afbclient, location_data *location);

#endif // GPS_H
