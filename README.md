# BakkesMod Hitbox Plugin
Draws a hitbox around your car during training.   Car type is auto-detected, and can also be set manually. 

## Known Issues

Auto-identification of car not functional with older versions of BakkesMod.  If you see an Octane hitbox on a car that's not Octane type, set the hitbox manually as described below.

## Installation

1. Copy `HitboxPlugin.dll` to `<steam_directory>/steamapps/common/rocketleague/Binaries/Win32/bakkesmod/plugins`
2. Copy `hitboxplugin.set` to `<steam_directory>/steamapps/common/rocketleague/Binaries/Win32/bakkesmod/plugins/settings`
3. In the game console run `plugin load hitboxplugin`

## Commands

**cl_soccar_showhitbox** [0|1] - turn hitbox on/off

**cl_soccar_listhitboxtypes** - see a list of the integer values for different car bodies.  Use this as a parameter to cl_soccar_sethitboxtype

**cl_soccar_sethitboxtype** <int> - set the hitbox to the specified value (e.g."cl_soccar_sethitboxtype 803" for Batmobile).

- when compiling the code; check and modify the include directories to point to the BakkesMod sdk, and the post-build step as well.

