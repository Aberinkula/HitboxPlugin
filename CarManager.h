#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "Hitbox.h"
#include <vector>
#include <string>

class CarManager
{
public:
	CarManager();
	static const std::string getHelpText();
	static Hitbox getHitbox(CARBODY car_type, CarWrapper& car);
	~CarManager();
};

