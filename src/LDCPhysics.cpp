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
	void LDCPhysics::init()
	{
		groundSensor.getSensor().addGroupsToCheck(LDGroup::Solid);

		body.setUserData(&getEntity());

		body.onDetection += [this](const DetectionInfo& mDI)
		{
			if(mDI.userData == nullptr) return;
			Entity* entity(static_cast<Entity*>(mDI.userData));
			onDetection(*entity);
		};
		body.onResolution += [this](const ResolutionInfo& mRI)
		{
			onResolution(mRI.resolution);

			lastResolution = mRI.resolution;
			if(mRI.resolution.x > 0) crushedLeft = crushedMax;
			else if(mRI.resolution.x < 0) crushedRight = crushedMax;
			if(mRI.resolution.y > 0) crushedTop = crushedMax;
			else if(mRI.resolution.y < 0) crushedBottom = crushedMax;
		};
		body.onPreUpdate += [this]
		{
			groundSensor.setPosition(body.getPosition() + Vec2i{0, body.getHeight() / 2});

			lastResolution = ssvs::zeroVec2i;
			if(crushedLeft > 0) --crushedLeft;
			if(crushedRight > 0) --crushedRight;
			if(crushedTop > 0) --crushedTop;
			if(crushedBottom > 0) --crushedBottom;
		};
	}
}
