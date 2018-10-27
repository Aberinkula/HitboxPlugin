#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include <vector>
#include <string>

class CarManager
{
public:
	CarManager();
	static const std::string getHelpText();
	static const std::vector<Vector> getHitboxPoints(CARBODY car_type);
	~CarManager();
};

