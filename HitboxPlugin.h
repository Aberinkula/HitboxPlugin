#pragma once
#include <memory>
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

/*
Colors the prediction line can have
*/
struct LineColor
{
	unsigned char r, g, b, a; //rgba can be a value of 0-255
};

/*Predicted point in 3d space*/
struct PredictedPoint
{
	/*Location of the predicted ball*/
	Vector location;
	/*States whether it as its highest point or bounces*/
	bool isApex = false;
	Vector apexLocation = { 0,0,0 };
	Vector velocity;
	Vector angVel;
};

class HitboxPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	//variables for settings cvars
	std::shared_ptr<bool> hitboxOn;
	std::shared_ptr<int> hitboxType;
	std::shared_ptr<bool> hitboxLook;
	std::shared_ptr<float> hitboxColorR;
	std::shared_ptr<float> hitboxColorG;
	std::shared_ptr<float> hitboxColorB;
	std::shared_ptr<float> testValue;
	LineColor colors[2] = { {0, 255, 0, 240}, {75, 0, 130, 240} };
	std::vector<Vector> hitbox;
public:
	HitboxPlugin();
	~HitboxPlugin();
	virtual void onLoad();
	virtual void onUnload();
	
	void OnFreeplayLoad(std::string eventName);
	void OnFreeplayDestroy(std::string eventName);
	void OnHitboxOnValueChanged(std::string oldValue, CVarWrapper cvar);
	void OnHitboxTypeChanged(std::string oldValue, CVarWrapper cvar);
	void OnHitboxLookChanged(std::string oldValue, CVarWrapper cvar);
	void Render(CanvasWrapper canvas);
};

// utility function
Vector Rotate(Vector aVec, double roll, double yaw, double pitch);
