#pragma once
#include <memory>
#pragma comment( lib, "pluginsdk.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "Hitbox.h"

class HitboxPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	std::shared_ptr<int> hitboxOn;
	std::shared_ptr<int> hitboxType;
	std::shared_ptr<LinearColor> hitboxColor;
	std::vector<Hitbox> hitboxes;

public:
	HitboxPlugin();
	~HitboxPlugin();
	virtual void onLoad();
	virtual void onUnload();
	
	void OnFreeplayLoad(std::string eventName);
	void OnFreeplayDestroy(std::string eventName);
	void OnHitboxOnValueChanged(std::string oldValue, CVarWrapper cvar);
	void OnHitboxTypeChanged(std::string oldValue, CVarWrapper cvar);
	void Render(CanvasWrapper canvas);
};

// Utility function for rotating around a point in 3D space.
extern inline Vector Rotate(Vector point, const Rotator& rotation, const Vector& location);