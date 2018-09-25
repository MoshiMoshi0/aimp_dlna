#include "stdafx.h"
#include "AimpDlnaConfig.h"

IAIMPCore *Config::core = nullptr; 
IAIMPServiceConfig *Config::configService = nullptr;

int Config::LogLevel = NPT_LOG_LEVEL_OFF;
int Config::ScanDuration = 2000;
int Config::ScanStop = 1;
int Config::StopDelay = 250;
wstring Config::UuidBlacklist = L"";