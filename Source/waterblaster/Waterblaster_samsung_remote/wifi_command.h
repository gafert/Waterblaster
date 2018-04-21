//
// Created by Michael Gafert on 02.12.17.
//

#ifndef EMBEDDED_SYSTEMS_WIFI_COMMAND_H
#define EMBEDDED_SYSTEMS_WIFI_COMMAND_H

#include <stdbool.h>

bool isWifiLeftActive;
bool isWifiRightActive;
bool isWifiShootActive;
bool isWifiUpActive;
bool isWifiDownActive;

void init_wifi_command();

#endif //EMBEDDED_SYSTEMS_WIFI_COMMAND_H
