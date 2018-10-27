#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include <vector>

class Hitbox
{

private:
	float length, width, height, x_offset, z_offset, angle;
	void computePoints();  //  fill in Vector of the 8 rectangular prism points
	std::vector<Vector> points;
	Vector PitchForward(Vector aVec, double angle);

public:
	Hitbox(float length, float width, float height, float x_offset, float z_offset, float angle);
	void getPoints(std::vector<Vector> & pts);
	~Hitbox();
};

