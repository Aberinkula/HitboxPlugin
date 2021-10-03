#include "Hitbox.h"

Hitbox::Hitbox(float length, float width, float height, float x_offset, float y_offset, float z_offset)
{
	this->length = length;
	this->width = width;
	this->height = height;
	this->x_offset = x_offset;
	this->y_offset = y_offset;
	this->z_offset = z_offset;

	computePoints();
}

void Hitbox::computePoints()
{
	points.clear();

	points.push_back(Vector(length, height, width));
	points.push_back(Vector(length, height, -width));
	points.push_back(Vector(-length, height, -width));
	points.push_back(Vector(-length, height, width));
	points.push_back(Vector(length, -height, width));
	points.push_back(Vector(length, -height, -width));
	points.push_back(Vector(-length, -height, -width));
	points.push_back(Vector(-length, -height, width));

	for (int i = 0; i < 8; i++)
	{
		points[i].X += x_offset;
		points[i].Z += y_offset; // plugin uses Y axis as up
		points[i].Y += z_offset; // Z axis of the game is up
	}
}

void Hitbox::getPoints(std::vector<Vector> & pts)
{
	pts.clear();

	for (int i = 0; i < 8; i++)
	{
		pts.push_back(points[i]);
	}
}

Hitbox::~Hitbox() { }