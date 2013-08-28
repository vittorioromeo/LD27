// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDGame.h"
#include "LDCPhysics.h"

using namespace ssvs;
using namespace sses;
using namespace std;
using namespace sf;
using namespace ssvsc;
using namespace ssvu;

namespace ld
{
	LDCPhysics::LDCPhysics(World& mWorld, bool mIsStatic, const Vec2i& mPosition, const Vec2i& mSize, bool mAffectedByGravity)
		: world(mWorld), body(world.create(mPosition, mSize, mIsStatic)), affectedByGravity{mAffectedByGravity} { }
	LDCPhysics::~LDCPhysics() { body.destroy(); }

	void LDCPhysics::init()
	{
		body.setUserData(&getEntity());

		body.onDetection += [&](const DetectionInfo& mDetectionInfo)
		{
			if(mDetectionInfo.userData == nullptr) return;
			Entity* entity(static_cast<Entity*>(mDetectionInfo.userData));
			onDetection(*entity);
		};
		body.onResolution += [&](const ResolutionInfo& mResolutionInfo)
		{
			onResolution(mResolutionInfo.resolution);

			lastResolution = mResolutionInfo.resolution;
			if(mResolutionInfo.resolution.x > 0) crushedLeft = crushedMax;
			else if(mResolutionInfo.resolution.x < 0) crushedRight = crushedMax;
			if(mResolutionInfo.resolution.y > 0) crushedTop = crushedMax;
			else if(mResolutionInfo.resolution.y < 0) crushedBottom = crushedMax;
		};
		body.onPreUpdate += [&]
		{
			lastResolution = {0, 0};
			if(crushedLeft > 0) --crushedLeft;
			if(crushedRight > 0) --crushedRight;
			if(crushedTop > 0) --crushedTop;
			if(crushedBottom > 0) --crushedBottom;
		};
	}

	void LDCPhysics::update(float)
	{
		if(affectedByGravity && body.getVelocity().y < maxVelocityY) body.applyForce(gravityForce);
	}
}
