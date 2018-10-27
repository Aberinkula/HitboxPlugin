#include "Hitbox.h"

Hitbox::Hitbox(float length, float width, float height, float x_offset, float z_offset, float angle) {
	this->length = length;
	this->width = width;
	this->height = height;
	this->x_offset = x_offset;
	this->z_offset = z_offset;
	this->angle = angle;
	computePoints();
}

Vector Hitbox::PitchForward(Vector aVec, double angle)
{
	//"As others have said the order in which you apply the rotations matters. Unity uses the local order Y - X - Z. The worldspace order is always the reverse, so Z - X - Y."

	float sz = sin(angle);
	float cz = cos(angle);

	aVec = Vector(aVec.X * cz - aVec.Y * sz, aVec.X * sz + aVec.Y * cz, aVec.Z); //1   pitch?

	return aVec;
}
void Hitbox::computePoints() {
	points.clear();
	points.push_back(Vector(length, height, width));
	points.push_back(Vector(length, height, -width));
	points.push_back(Vector(-length, height, -width));
	points.push_back(Vector(-length, height, width));
	points.push_back(Vector(length, -height, width));
	points.push_back(Vector(length, -height, -width));
	points.push_back(Vector(-length, -height, -width));
	points.push_back(Vector(-length, -height, width));
	
	for (int i = 0; i < 8; i++) {
		// TODO: rotate points by angle
		points[i] = PitchForward(points[i],  angle / 360.0 * 2.0 * 3.14159);
		points[i].X += x_offset;
		points[i].Y += z_offset;
	}
}

void Hitbox::getPoints(std::vector<Vector> & pts)
{
	pts.clear();
	for (int i = 0; i < 8; i++)
		pts.push_back(points[i]);
}

Hitbox::~Hitbox()
{
}
