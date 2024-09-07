#pragma once

class Bones {
public:
	bool BuildBones(Player* target, int mask, BoneArray* out, bool rebuilt, bool interpolate = false);
	bool RebuiltSetupBones(Player* target, int mask, BoneArray* out);
};

extern Bones g_bones;