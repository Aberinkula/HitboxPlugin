#include "CarManager.h"
#include "Hitbox.h"

CarManager::CarManager()
{
}

const std::vector<Vector> CarManager::getHitboxPoints(CARBODY car_type)
{
	Hitbox *hitbox;
	switch (car_type) {
	// OCTANE type ---------------------------------------------------------------
	case CAR_OCTANE:
	case CAR_OCTANEZSR:
	case CAR_BACKFIRE:
	case CAR_ZIPPY:
	case CAR_ROADHOG:
	case CAR_ROADHOGXL:
	case CAR_TAKUMI:
	case CAR_VULCAN:
	case CAR_TAKUMIRXT:
	case CAR_BONESHAKER:
	case CAR_TRITON:
	case CAR_MERC:
	case CAR_SCARAB:
	case CAR_GIZMO:
	case CAR_DELOREAN:
	case CAR_PROTEUS:
	case CAR_GROG:
	case CAR_ARMADILLO: //gears of war car, XBOX exclusive?
	case CAR_MARAUDER:
		hitbox = new Hitbox(118.0074000f / 2.0f, 84.1994100f / 2.0f, 36.1590700f / 2.0f, 13.87566, 20.75499, -0.55);
		break;

	// BREAKOUT type ---------------------------------------------------------------
	case CAR_BREAKOUT:
	case CAR_BREAKOUTTYPES:
	case CAR_ANIMUSGP:
		hitbox = new Hitbox(65.746178f, 40.260502f, 15.15f, 12.5f, 11.75f, -0.98f);
		break;

	// BATMOBILE ---------------------------------------------------------------
	case CAR_BATMOBILE:
		hitbox = new Hitbox(64.409989f, 42.335182f, 14.697201f, 9.008572f, 12.0942f, -0.34f);
		break;

	// DOMINUS type ---------------------------------------------------------------
	case CAR_DOMINUS:
	case CAR_HOTSHOT:
	case CAR_DOMINUSGT:
	case CAR_ICECHARGER:
	case CAR_AFTERSHOCK:
	case CAR_MASAMUNE:
	case CAR_RIPPER:
		hitbox = new Hitbox(63.96339f, 41.639977f, 15.65f, 9.0f, 15.75f, -0.96f);
		break;

	// HYBRID type ---------------------------------------------------------------
	case CAR_VENOM:
	case CAR_XDEVIL:
	case CAR_XDEVILMK2:
	case CAR_ENDO:
	case CAR_ESPER:
	case CAR_JOGER619RS:
		hitbox = new Hitbox(63.509594f, 41.093933f, 17.079536f, 13.87566f, 20.75499f, -0.55f);
		break;

	// PLANK type    ---------------------------------------------------------------
	case CAR_MANTIS:
	case CAR_TWINMILL:
	case CAR_PALADIN:
	case CAR_CENTIO:
		hitbox = new Hitbox(64.409889f, 42.335182f, 14.697201f, 9.008572f, 12.0942f, -0.36f);
		break;

	// UNKNOWN types : return OCTANE for now.  TODO: fill these in
	case CAR_SWEETTOOTH: //Will just spawn merc i guess, ps4 exclusive
	case CAR_WARTHOG: //Halo car, XBOX exclusive
	case 0:
	default:
		hitbox = new Hitbox(118.0074000f / 2.0f, 84.1994100f / 2.0f, 36.1590700f / 2.0f, 13.87566, 20.75499, -0.55);
		break;
	}	
	std::vector<Vector> pts;
	hitbox->getPoints(pts);
	delete hitbox;
	return pts;
}


CarManager::~CarManager()
{
}

const std::string CarManager::getHelpText() {

	return std::string("CAR_BACKFIRE = 21, \n\
		CAR_BREAKOUT = 22,\n\
		CAR_BREAKOUTTYPES = 1416,  \n\
		CAR_OCTANE = 23, \n\
		CAR_OCTANEZSR = 1568, \n\
		CAR_PALADIN = 24, \n\
		CAR_ROADHOG = 25, \n\
		CAR_ROADHOGXL = 1300, \n\
		CAR_GIZMO = 26, \n\
		CAR_SWEETTOOTH = 27,  \n\
		CAR_XDEVIL = 28, \n\
		CAR_XDEVILMK2 = 1159, \n\
		CAR_HOTSHOT = 29, \n\
		CAR_MERC = 30, \n\
		CAR_VENOM = 31, \n\
		CAR_TAKUMI = 402, \n\
		CAR_TAKUMIRXT = 1295, \n\
		CAR_DOMINUS = 403, \n\
		CAR_DOMINUSGT = 1018, \n\
		CAR_SCARAB = 404, \n\
		CAR_ZIPPY = 523, \n\
		CAR_DELOREAN = 597, \n\
		CAR_RIPPER = 600, \n\
		CAR_GROG = 607, \n\
		CAR_ARMADILLO = 625,  \n\
		CAR_WARTHOG = 723, \n\
		CAR_BATMOBILE = 803, \n\
		CAR_MASAMUNE = 1171, \n\
		CAR_MARAUDER = 1172, \n\
		CAR_AFTERSHOCK = 1286, \n\
		CAR_ESPER = 1317, \n\
		CAR_PROTEUS = 1475, \n\
		CAR_TRITON = 1478, \n\
		CAR_VULCAN = 1533, \n\
		CAR_TWINMILL = 1603, \n\
		CAR_BONESHAKER = 1623, \n\
		CAR_ENDO = 1624, \n\
		CAR_ICECHARGER = 1675, \n\
		CAR_MANTIS = 1691, \n\
		CAR_JOGER619RS = 1856, \n\
		CAR_CENTIO = 1919, \n\
		CAR_ANIMUSGP = 1932");
}
