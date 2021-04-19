#include "LogManager.h"

Destroyer<LogManager> LogManager::_destroyer;
LogManager *LogManager::_instance = NULL;