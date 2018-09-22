#include "stdafx.h"
#include "AimpDlnaConfig.h"

IAIMPCore *Config::core = nullptr; 
IAIMPServiceConfig *Config::configService = nullptr;

int Config::DebugLevel = NPT_LOG_LEVEL_OFF;