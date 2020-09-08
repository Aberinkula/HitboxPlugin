#define LINMATH_H //Conflicts with linmath.h if we done declare this here

#include "HitboxPlugin.h"
#include "Hitbox.h"
#include "CarManager.h"
#include "bakkesmod/wrappers/GameEvent/ServerWrapper.h"
#include "bakkesmod/wrappers/GameObject/BallWrapper.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod/wrappers/arraywrapper.h"
#include "RenderingTools/Objects/Circle.h"
#include "RenderingTools/Objects/Frustum.h"
#include "RenderingTools/Extra/WrapperStructsExtensions.h"
#include "RenderingTools/Extra/RenderingMath.h"
#include <sstream>

BAKKESMOD_PLUGIN(HitboxPlugin, "Hitbox plugin", "2.0", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

HitboxPlugin::HitboxPlugin()
{

}

HitboxPlugin::~HitboxPlugin()
{
}

void HitboxPlugin::onLoad()
{
	hitboxOn = std::make_shared<bool>(true);
	cvarManager->registerCvar("cl_soccar_showhitbox", "0", "Show Hitbox", true, true, 0, true, 1).bindTo(hitboxOn);
	cvarManager->getCvar("cl_soccar_showhitbox").addOnValueChanged(std::bind(&HitboxPlugin::OnHitboxOnValueChanged, this, std::placeholders::_1, std::placeholders::_2));

	hitboxType = std::make_shared<int>(0);
	cvarManager->registerCvar("cl_soccar_sethitboxtype", "0", "Set Hitbox Car Type", true, true, 0, true, 32767, false).bindTo(hitboxType);
	cvarManager->getCvar("cl_soccar_sethitboxtype").addOnValueChanged(std::bind(&HitboxPlugin::OnHitboxTypeChanged, this, std::placeholders::_1, std::placeholders::_2));


	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartPlayTest", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameInfo_Replay_TA.InitGame", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameInfo_Replay_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function Engine.Actor.SpawnInstance", [this](std::string eventName) {
		this->OnHitboxTypeChanged("", cvarManager->getCvar("cl_soccar_sethitboxtype"));
		});

	cvarManager->registerNotifier("cl_soccar_listhitboxtypes", [this](std:: vector<std::string> params) {
		cvarManager->log(CarManager::getHelpText());
	}, "List all hitbox integer types, use these values as parameters for cl_soccar_sethitboxtype", PERMISSION_ALL);
	
}

void HitboxPlugin::OnFreeplayLoad(std::string eventName)
{
	// get the 8 hitbox points for current car type
	hitboxes.clear();  // we'll reinitialize this in Render, for the first few ticks of free play, the car is null
	cvarManager->log(std::string("OnFreeplayLoad") + eventName);
	if (  *hitboxOn ) {
		gameWrapper->RegisterDrawable(std::bind(&HitboxPlugin::Render, this, std::placeholders::_1));
	}	
}

void HitboxPlugin::OnFreeplayDestroy(std::string eventName)
{
	gameWrapper->UnregisterDrawables();
}

void HitboxPlugin::OnHitboxOnValueChanged(std::string oldValue, CVarWrapper cvar)
{
	if (cvar.getBoolValue() && (gameWrapper->IsInGame() || gameWrapper->IsInReplay())) {
		OnFreeplayLoad("Load");
	}
	else
	{
		OnFreeplayDestroy("Destroy");
	}
}

void HitboxPlugin::OnHitboxTypeChanged(std::string oldValue, CVarWrapper cvar) {
	hitboxes.clear();
}


#include <iostream>     // std::cout
#include <fstream> 

 Vector Rotate(Vector aVec, double roll, double yaw, double pitch)
{

	 // this rotate is kind of messed up, because UE's xyz coordinates didn't match the axes i expected
	/*
	float sx = sin(pitch);
	float cx = cos(pitch);
	float sy = sin(yaw);
	float cy = cos(yaw);
	float sz = sin(roll);
	float cz = cos(roll);
	*/
	float sx = sin(roll);
	float cx = cos(roll);
	float sy = sin(yaw);
	float cy = cos(yaw);
	float sz = sin(pitch);
	float cz = cos(pitch);

	aVec = Vector(aVec.X, aVec.Y * cx - aVec.Z * sx, aVec.Y * sx + aVec.Z * cx);  //2  roll?

	 
	aVec = Vector(aVec.X * cz - aVec.Y * sz, aVec.X * sz + aVec.Y * cz, aVec.Z); //1   pitch?
	aVec = Vector(aVec.X * cy + aVec.Z * sy, aVec.Y, -aVec.X * sy + aVec.Z * cy);  //3  yaw?

	// ugly fix to change coordinates to Unreal's axes
	float tmp = aVec.Z;
	aVec.Z = aVec.Y;
	aVec.Y = tmp;
	return aVec;
}


void HitboxPlugin::Render(CanvasWrapper canvas)
{
	int ingame = (gameWrapper->IsInGame()) ? 1 : (gameWrapper->IsInReplay()) ? 2 : 0;
	if (*hitboxOn && ingame)
	{

		ServerWrapper game = (ingame == 1)? gameWrapper->GetGameEventAsServer(): gameWrapper->GetGameEventAsReplay();

		if (game.IsNull())
			return;
		ArrayWrapper<CarWrapper> cars = game.GetCars();
		RT::Frustum frust{ canvas, gameWrapper->GetCamera() };
		std::vector<Vector> hitbox;
		int car_i = 0;
		for (auto car : cars) {
			if (car.IsNull())
				continue;
			if (hitboxes.size() <= car_i) { // initialize hitboxes 
				hitboxes.push_back(CarManager::getHitbox(static_cast<CARBODY>(*hitboxType), car));
			}
			canvas.SetColor(255, 255, 0, 200);

			Vector v = car.GetLocation();
			Rotator r = car.GetRotation();

			double dPitch = (double)r.Pitch / 32768.0*3.14159;
			double dYaw = (double)r.Yaw / 32768.0*3.14159;
			double dRoll = (double)r.Roll / 32768.0*3.14159;

			Vector2 carLocation2D = canvas.Project(v);
			Vector2 hitbox2D[8];
			
			hitboxes.at(car_i).getPoints(hitbox);
			for (int i = 0; i < 8; i++) {
				hitbox2D[i] = canvas.Project(Rotate(hitbox[i], dRoll, -dYaw, dPitch) + v);
			}

			canvas.DrawLine(hitbox2D[0], hitbox2D[1]);
			canvas.DrawLine(hitbox2D[1], hitbox2D[2]);
			canvas.DrawLine(hitbox2D[2], hitbox2D[3]);
			canvas.DrawLine(hitbox2D[3], hitbox2D[0]);
			canvas.DrawLine(hitbox2D[4], hitbox2D[5]);
			canvas.DrawLine(hitbox2D[5], hitbox2D[6]);
			canvas.DrawLine(hitbox2D[6], hitbox2D[7]);
			canvas.DrawLine(hitbox2D[7], hitbox2D[4]);
			canvas.DrawLine(hitbox2D[0], hitbox2D[4]);
			canvas.DrawLine(hitbox2D[1], hitbox2D[5]);
			canvas.DrawLine(hitbox2D[2], hitbox2D[6]);
			canvas.DrawLine(hitbox2D[3], hitbox2D[7]);

			canvas.SetPosition(carLocation2D.minus((Vector2{ 10,10 })));
			canvas.FillBox((Vector2{ 20, 20 }));


			auto sim = car.GetVehicleSim();
			auto wheels = sim.GetWheels();
			if (wheels.IsNull()) continue;
			Quat car_rot = RT::RotatorToQuat(r);
			Vector turn_axis = RT::RotateVectorWithQuat(Vector{ 0.f, 0.f, 1.f }, car_rot);
			Quat upright_rot = RT::AngleAxisRotation(3.14159f / 2.0f, Vector{ 1.f, 0.f, 0.f });
			for (auto wheel : wheels)
			{
				Vector loc = wheel.GetLocalRestPosition() - Vector(0.f, 0.f, wheel.GetSuspensionDistance());
				loc = RT::RotateVectorWithQuat(loc, car_rot);
				loc = loc + v;

				Quat turn_rot = RT::AngleAxisRotation(wheel.GetSteer2(), Vector{ 0.f, 0.f, 1.f });
				Quat final_rot = car_rot * turn_rot * upright_rot;

				RT::Circle circ{ loc, final_rot, wheel.GetWheelRadius() };
				
				circ.Draw(canvas, frust);
			}

			car_i++;
		}
	}
}

void HitboxPlugin::onUnload()
{
}

