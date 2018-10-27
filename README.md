# BakkesMod Hitbox Plugin
Draws a hitbox around your car during training.   Car type auto-detection not working yet, must be set manually. Hitbox defaults to Octane.

## commands

**cl_soccar_showhitbox** [0|1] - turn hitbox on/off

**cl_soccar_listhitboxtypes** - see a list of the integer values for different car bodies.  Use this as a parameter to cl_soccar_sethitboxtype

**cl_soccar_sethitboxtype** <int> - set the hitbox to the specified value (e.g."cl_soccar_sethitboxtype 803" for Batmobile).

- when compiling the code; check and modify the include directories to point to the BakkesMod sdk, and the post-build step as well.

