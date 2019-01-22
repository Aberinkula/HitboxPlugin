#define LINMATH_H //Conflicts with linmath.h if we done declare this here

#include "HitboxPlugin.h"
#include "Hitbox.h"
#include "CarManager.h"
#include "bakkesmod/wrappers/GameEvent/ServerWrapper.h"
#include "bakkesmod/wrappers/GameObject/BallWrapper.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod/wrappers/arraywrapper.h"
#include <sstream>

BAKKESMOD_PLUGIN(HitboxPlugin, "Test hitbox plugin", "1.0", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

HitboxPlugin::HitboxPlugin()
{

}

HitboxPlugin::~HitboxPlugin()
{
}


double bH = 0; 
double cH = 0;
bool boxOn = true;

void HitboxPlugin::onLoad()
{
	//set .set cvar
	hitboxLook = make_shared<bool>(true);
	//set the first string in the .set
	cvarManager->registerCvar("cl_soccar_sethitboxlook", "0", "Set Hitbox Look", true, true, 0, true, 1).bindTo(hitboxLook);

	hitboxColorR = make_shared<float>(0);
	cvarManager->registerCvar("cl_soccar_sethitboxR", "0", "Set Hitbox Color Red", true, true, 0, true, 255).bindTo(hitboxColorR);

	hitboxColorG = make_shared<float>(0);
	cvarManager->registerCvar("cl_soccar_sethitboxG", "0", "Set Hitbox Color Green", true, true, 0, true, 255).bindTo(hitboxColorG);

	hitboxColorB = make_shared<float>(0);
	cvarManager->registerCvar( "cl_soccar_sethitboxB", "0", "Set Hitbox Color Blue", true, true, 0, true, 255).bindTo(hitboxColorB);

	testValue = make_shared<float>(0.0);
	cvarManager->registerCvar("cl_soccar_setTestV", "0", "Set Hitbox Color Blue", true, true, -500.0, true, 500.0).bindTo(testValue);

	hitboxOn = make_shared<bool>(true);
	cvarManager->registerCvar("cl_soccar_showhitbox", "0", "Show Hitbox", true, true, 0, true, 1).bindTo(hitboxOn);
	cvarManager->getCvar("cl_soccar_showhitbox").addOnValueChanged(std::bind(&HitboxPlugin::OnHitboxOnValueChanged, this, std::placeholders::_1, std::placeholders::_2));

	hitboxType = make_shared<int>(0);
	cvarManager->registerCvar("cl_soccar_sethitboxtype", "0", "Set Hitbox Car Type", true, true, 0, true, 32767, false).bindTo(hitboxType);
	cvarManager->getCvar("cl_soccar_sethitboxtype").addOnValueChanged(std::bind(&HitboxPlugin::OnHitboxTypeChanged, this, std::placeholders::_1, std::placeholders::_2));

	

	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.OnInit", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartPlayTest", bind(&HitboxPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", bind(&HitboxPlugin::OnFreeplayDestroy, this, std::placeholders::_1));

	cvarManager->registerNotifier("cl_soccar_listhitboxtypes", [this](vector<string> params) {
		cvarManager->log(CarManager::getHelpText());
	}, "List all hitbox integer types, use these values as parameters for cl_soccar_sethitboxtype", PERMISSION_ALL);

}

void HitboxPlugin::OnFreeplayLoad(std::string eventName)
{
	// get the 8 hitbox points for current car type
	hitbox.clear();  // we'll reinitialize this in Render, for the first few ticks of free play, the car is null
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
	if (cvar.getBoolValue() && gameWrapper->IsInGame()) {
		OnFreeplayLoad("Load");
	}
	else
	{
		OnFreeplayDestroy("Destroy");
	}
}



void HitboxPlugin::OnHitboxTypeChanged(std::string oldValue, CVarWrapper cvar) {
	hitbox = CarManager::getHitboxPoints(static_cast<CARBODY>(cvar.getIntValue()), *gameWrapper);
}

void HitboxPlugin::OnHitboxLookChanged(std::string oldValue, CVarWrapper cvar) {
	if (cvar.getBoolValue() && gameWrapper->IsInGame()) {
		OnFreeplayLoad("Load");
	}
	else
	{
		OnFreeplayDestroy("Destroy");
	}
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

 void DrawRectangle(Vector2 point1, Vector2 point2, Vector2 point3, Vector2 point4, LinearColor color,CanvasWrapper canvas)
 {
	 canvas.FillTriangle(point1, point2, point3, color);
	 canvas.FillTriangle(point1, point4, point3, color);
 }

void HitboxPlugin::Render(CanvasWrapper canvas)
{
	
	if (*hitboxOn && gameWrapper->IsInGame())
	{
		LinearColor tColor;
		tColor.B = *hitboxColorR;
		tColor.G = *hitboxColorG;
		tColor.R = *hitboxColorB;
		ServerWrapper game = gameWrapper->GetGameEventAsServer();

		if (game.IsNull())
			return;
		ArrayWrapper<CarWrapper> cars = game.GetCars();
		 
		if (cars.Count() > 0) {
			CarWrapper car = cars.Get(0);
			if (car.IsNull())
				return;
			if (hitbox.size() == 0) { // initialize hitbox 
				hitbox = CarManager::getHitboxPoints(static_cast<CARBODY>(*hitboxType), *gameWrapper);
			}
			canvas.SetColor(0, 0, 0, 200);

			Vector v = car.GetLocation();
			Rotator r = car.GetRotation();

			double dPitch = (double)r.Pitch / 32764.0*3.14159;
			double dYaw = (double)r.Yaw / 32764.0*3.14159;
			double dRoll = (double)r.Roll / 32764.0*3.14159;

			Vector2 carLocation2D = canvas.Project(v);
			Vector2 hitbox2D[8];
			for (int i = 0; i < 8; i++) {
				hitbox2D[i] = canvas.Project(Rotate(hitbox[i], dRoll, -dYaw, dPitch) + v);
			}
			BallWrapper ball = game.GetBall();
			double ballRadius = ball.GetDrawScale();
			double ballHeight = ball.GetLocation().Z;
			double carHeight = car.GetRBState().Location.Z;
			if (bH == 0) {
				bH = ballHeight;
			}
			if (cH == 0) {
				cH = carHeight;
			}
			//fix ball/car ratio...
			canvas.SetPosition({ 300, 300 });
			canvas.SetColor(255, 0, 0, 255);
			double diffC = (bH * 2 - cH);
			std::ostringstream strs;
			strs << diffC;
			std::string str = strs.str();
			canvas.DrawString(str);
			canvas.SetPosition({ 300, 325 });
			std::ostringstream strs1;
			strs1 << carHeight;
			std::string str1 = strs1.str();
			canvas.DrawString(str1);

			canvas.SetPosition({300, 350});
			std::ostringstream strs2;
			strs2 << ballHeight;
			std::string str2 = strs2.str();
			canvas.DrawString(str2);

			//ball height - car height = difference between ball and car constatnt
			//if (ballHeight > carHeight + diffC) {
			if (false) {
				canvas.DrawLine({ 25,25 }, { 50,50 }, 250);
			}
			else {
				if (!*hitboxLook) {
					for (int i = 0; i < 8; i++) {
						//y
						if (i == 3 || ((i > 3) && ((i - 3) % 4 == 0))) {
							if (hitbox2D[i].X)
								canvas.DrawLine(hitbox2D[i], hitbox2D[i - 3], 15);
						}
						//x
						else {
							canvas.DrawLine(hitbox2D[i], hitbox2D[i + 1], 15);
						}
						//z
						if (i < 4) {
							canvas.DrawLine(hitbox2D[i], hitbox2D[i + 4], 15);
						}


					}
				}
				if (*hitboxLook) {
					DrawRectangle(hitbox2D[0], hitbox2D[1], hitbox2D[2], hitbox2D[3], tColor, canvas);
					DrawRectangle(hitbox2D[4], hitbox2D[5], hitbox2D[6], hitbox2D[7], tColor, canvas);
					DrawRectangle(hitbox2D[0], hitbox2D[4], hitbox2D[5], hitbox2D[1], tColor, canvas);
					DrawRectangle(hitbox2D[0], hitbox2D[4], hitbox2D[7], hitbox2D[3], tColor, canvas);
					DrawRectangle(hitbox2D[7], hitbox2D[3], hitbox2D[2], hitbox2D[1], tColor, canvas);
					DrawRectangle(hitbox2D[1], hitbox2D[5], hitbox2D[6], hitbox2D[2], tColor, canvas);
				}
			}
			

			//canvas.DrawLine(hitbox2D[0], hitbox2D[1], 10);
			//canvas.SetColor(0, 255, 0, 200); //green
			//canvas.DrawLine(hitbox2D[1], hitbox2D[2],10);
			//canvas.SetColor(0, 255, 255, 200); //teal
			//canvas.DrawLine(hitbox2D[2], hitbox2D[3],10);
			//canvas.SetColor(0, 0, 255, 200);//blue
			//canvas.DrawLine(hitbox2D[3], hitbox2D[0],10);
			//canvas.SetColor(255, 0, 255, 200); //pink
			//canvas.DrawLine(hitbox2D[4], hitbox2D[5],10);
			//canvas.SetColor(255, 0, 0, 200); //red
			//canvas.DrawLine(hitbox2D[5], hitbox2D[6],10);
			//canvas.SetColor(128, 0, 255, 200); //purple
			//canvas.DrawLine(hitbox2D[6], hitbox2D[7],10);
			//canvas.SetColor(255, 128, 0, 200); //orange
			//canvas.DrawLine(hitbox2D[7], hitbox2D[4],10);
			//canvas.SetColor(128, 128, 128, 200); //grey
			//canvas.DrawLine(hitbox2D[0], hitbox2D[4],10);
			//canvas.SetColor(85, 107, 47, 200); //dark olive green
			//canvas.DrawLine(hitbox2D[1], hitbox2D[5],10);
			//canvas.SetColor(245, 222, 179, 200); //wheat
			//canvas.DrawLine(hitbox2D[2], hitbox2D[6],10);
			//canvas.SetColor(255, 255, 255, 200);//white
			//canvas.DrawLine(hitbox2D[3], hitbox2D[7],10);
			//canvas.SetColor(128, 0, 0, 200); //maroon

			canvas.SetPosition(carLocation2D.minus({ 10,10 }));
			canvas.FillBox({ 20, 20 });
			return;
		}
	}
}

void HitboxPlugin::onUnload()
{
}

