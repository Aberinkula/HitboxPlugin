#define LINMATH_H //Conflicts with linmath.h if we done declare this here

// Useful mathy macros.
#define PI 						(3.1415926535897932f)
#define Rotation360				(65535)
#define Rotation180				(32768)
#define RotationToRadians		(PI / Rotation180)

#include "HitboxPlugin.h"
#include "Hitbox.h"
#include "CarManager.h"
#include "bakkesmod/wrappers/includes.h"
#include "RenderingTools/Objects/Circle.h"
#include "RenderingTools/Objects/Frustum.h"
#include "RenderingTools/Objects/Line.h"
#include "RenderingTools/Objects/Sphere.h"
#include "RenderingTools/Extra/WrapperStructsExtensions.h"
#include "RenderingTools/Extra/RenderingMath.h"
#include <sstream>
#include <iostream>
#include <fstream> 

BAKKESMOD_PLUGIN(HitboxPlugin, "Hitbox plugin", "2.2", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

HitboxPlugin::HitboxPlugin() { }

HitboxPlugin::~HitboxPlugin() { }

void HitboxPlugin::onLoad()
{
	hitboxOn = std::make_shared<int>(0);
	cvarManager->registerCvar("cl_soccar_showhitbox", "0", "Show Hitbox", true, true, 0, true, 3).bindTo(hitboxOn);
	cvarManager->getCvar("cl_soccar_showhitbox").addOnValueChanged(std::bind(&HitboxPlugin::OnHitboxOnValueChanged, this, std::placeholders::_1, std::placeholders::_2));

	hitboxType = std::make_shared<int>(0);
	cvarManager->registerCvar("cl_soccar_sethitboxtype", "0", "Set Hitbox Car Type", true, true, 0, true, 32767, false).bindTo(hitboxType);
	cvarManager->getCvar("cl_soccar_sethitboxtype").addOnValueChanged(std::bind(&HitboxPlugin::OnHitboxTypeChanged, this, std::placeholders::_1, std::placeholders::_2));

	hitboxColor = std::make_shared<LinearColor>(LinearColor{ 0.f, 0.f, 0.f, 0.f });
	cvarManager->registerCvar("cl_soccar_hitboxcolor", "#FFFF00", "Color of the hitbox visualization.", true).bindTo(hitboxColor);

	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartPlayTest", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameInfo_Replay_TA.InitGame", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Replay_TA.EventPostTimeSkip", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameInfo_Replay_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Replay_TA.EventSpawned", [this](std::string eventName) {
		this->OnHitboxTypeChanged("", cvarManager->getCvar("cl_soccar_sethitboxtype"));
	});

	cvarManager->registerNotifier("cl_soccar_listhitboxtypes", [this](std:: vector<std::string> params) {
		cvarManager->log(CarManager::getHelpText());
	}, "List all hitbox integer types, use these values as parameters for cl_soccar_sethitboxtype", PERMISSION_ALL);
}

void HitboxPlugin::OnFreeplayLoad(std::string eventName)
{
	// Get the 8 hitbox points for current car type.
	hitboxes.clear(); // We'll reinitialize this in Render, for the first few ticks of free play, the car is null.
	cvarManager->log("OnFreeplayLoad: " + eventName);

	if (*hitboxOn)
	{
		gameWrapper->RegisterDrawable(std::bind(&HitboxPlugin::Render, this, std::placeholders::_1));
	}	
}

void HitboxPlugin::OnFreeplayDestroy(std::string eventName)
{
	gameWrapper->UnregisterDrawables();
}

void HitboxPlugin::OnHitboxOnValueChanged(std::string oldValue, CVarWrapper cvar)
{
	int inGame = (gameWrapper->IsInReplay()) ? 2 : ((gameWrapper->IsInOnlineGame()) ? 0 : ((gameWrapper->IsInGame()) ? 1 : 0));
	//cvarManager->log("OnHitboxValueChanged: " + std::to_string(ingame));

	if (cvar.getIntValue() & inGame)
	{
		OnFreeplayLoad("Load");
	}
	else
	{
		OnFreeplayDestroy("Destroy");
	}
}

void HitboxPlugin::OnHitboxTypeChanged(std::string oldValue, CVarWrapper cvar)
{
	hitboxes.clear();
}

void HitboxPlugin::Render(CanvasWrapper canvas)
{
	int inGame = (gameWrapper->IsInGame()) ? 1 : (gameWrapper->IsInReplay()) ? 2 : 0;

	if (*hitboxOn & inGame)
	{
		if (gameWrapper->IsInOnlineGame() && inGame != 2) { return; }

		ServerWrapper game = gameWrapper->GetCurrentGameState();
		if (game.IsNull()) { return; }

		CameraWrapper camera = gameWrapper->GetCamera();
		if (camera.IsNull()) return;

		RT::Frustum frust{ canvas, camera };
		std::vector<Vector> hitbox;
		ArrayWrapper<CarWrapper> cars = game.GetCars();

		if (cars.Count() < hitboxes.size())
		{
			hitboxes.clear();
		}

		int carIndex = 0;

		for (CarWrapper& car : cars)
		{
			if (car.IsNull()) { continue; }

			// Initialize all hitboxes in the current game.
			if (hitboxes.size() <= carIndex)
			{
				hitboxes.push_back(CarManager::getHitbox(static_cast<CARBODY>(*hitboxType), car));
			}

			canvas.SetColor(*hitboxColor);	
			hitboxes.at(carIndex).getPoints(hitbox);

			// On the first tick this gets hooked, the extent/offset returned is still 0.
			// So we skip this tick and throw the data away to get it again next frame.
			if (fabs(hitbox[0].Z - hitbox[1].Z) < 0.01f)
			{
				hitboxes.clear();
				return;
			}

			Rotator rot = car.GetRotation();
			Vector loc = car.GetLocation();

			std::vector<Vector> cubeFaces;

			// Rotating the hitbox along with the cars rotation and location.
			for (size_t i = 0; i < hitbox.size(); i++)
			{
				cubeFaces.push_back(Rotate(hitbox[i], rot, loc));
			}

			CameraWrapper camera = gameWrapper->GetCamera();
			if (camera.IsNull()) { return; }

			RT::Frustum frust{ canvas, camera };

			RT::Line(cubeFaces[0], cubeFaces[1], 1.f).DrawWithinFrustum(canvas, frust); // Front top
			RT::Line(cubeFaces[1], cubeFaces[2], 1.f).DrawWithinFrustum(canvas, frust); // Left top
			RT::Line(cubeFaces[2], cubeFaces[3], 1.f).DrawWithinFrustum(canvas, frust); // Back top
			RT::Line(cubeFaces[3], cubeFaces[0], 1.f).DrawWithinFrustum(canvas, frust); // Right top

			RT::Line(cubeFaces[4], cubeFaces[5], 1.f).DrawWithinFrustum(canvas, frust); // Front bottom
			RT::Line(cubeFaces[5], cubeFaces[6], 1.f).DrawWithinFrustum(canvas, frust); // Left bottom
			RT::Line(cubeFaces[6], cubeFaces[7], 1.f).DrawWithinFrustum(canvas, frust); // Back bottom
			RT::Line(cubeFaces[7], cubeFaces[4], 1.f).DrawWithinFrustum(canvas, frust); // Right bottom

			RT::Line(cubeFaces[0], cubeFaces[4], 1.f).DrawWithinFrustum(canvas, frust); // Front right
			RT::Line(cubeFaces[1], cubeFaces[5], 1.f).DrawWithinFrustum(canvas, frust); // Front left
			RT::Line(cubeFaces[2], cubeFaces[6], 1.f).DrawWithinFrustum(canvas, frust); // Back left
			RT::Line(cubeFaces[3], cubeFaces[7], 1.f).DrawWithinFrustum(canvas, frust); // Back right

			float diff = (camera.GetLocation() - loc).magnitude();
			Quat car_rot = RotatorToQuat(rot);

			// Draws a sphere on the car where the center of mass is.
			if (diff < 1000.f)
			{
				RT::Sphere(loc, car_rot, 2.f).Draw(canvas, frust, camera.GetLocation(), 10);
			}

			VehicleSimWrapper sim = car.GetVehicleSim();
			ArrayWrapper<WheelWrapper> wheels = sim.GetWheels();

			if (wheels.IsNull()) { continue; }

			Vector turn_axis = RotateVectorWithQuat(Vector(0.f, 0.f, 1.f), car_rot);
			Quat upright_rot = RT::AngleAxisRotation(PI / 2.0f, Vector(1.f, 0.f, 0.f));

			for (WheelWrapper& wheel : wheels)
			{
				Vector wheelLoc = wheel.GetLocalRestPosition() - Vector(0.f, 0.f, wheel.GetSuspensionDistance());
				wheelLoc = RotateVectorWithQuat(wheelLoc, car_rot);
				wheelLoc += loc;

				Quat turn_rot = RT::AngleAxisRotation(wheel.GetSteer2(), Vector(0.f, 0.f, 1.f));
				Quat final_rot = car_rot * turn_rot * upright_rot;

				RT::Circle circ{ wheelLoc, final_rot, wheel.GetWheelRadius() };
				
				circ.Draw(canvas, frust);
			}

			carIndex++;
		}
	}
}

void HitboxPlugin::onUnload() { }

Vector Rotate(Vector point, const Rotator& rotation, const Vector& location)
{
	double pitch = (double)rotation.Pitch / Rotation180 * PI;
	double yaw = (double)rotation.Yaw / Rotation180 * PI;
	double roll = (double)rotation.Roll / Rotation180 * PI;

	float sz = sin(pitch);
	float cz = cos(pitch);
	float sy = sin(-yaw);
	float cy = cos(-yaw);
	float sx = sin(roll);
	float cx = cos(roll);

	point = Vector(point.X, point.Y * cx - point.Z * sx, point.Y * sx + point.Z * cx); // Roll
	point = Vector(point.X * cz - point.Y * sz, point.X * sz + point.Y * cz, point.Z); // Pitch
	point = Vector(point.X * cy + point.Z * sy, point.Y, -point.X * sy + point.Z * cy ); // Yaw

	// Switch coordinates to match Unreal Engines's axes.
	float tmp = point.Z;
	point.Z = point.Y;
	point.Y = tmp;

	// Adding the final location to the rotated vector.
	point.X += location.X;
	point.Y += location.Y;
	point.Z += location.Z;

	return point;
}