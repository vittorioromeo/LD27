// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDGame.hpp"
#include "LDCPhysics.hpp"

using namespace ssvs;
using namespace sses;
using namespace std;
using namespace sf;
using namespace ssvsc;
using namespace ssvu;

namespace ld
{
	LDCPhysics::LDCPhysics(sses::Entity& mE, World& mWorld, bool mIsStatic, const ssvs::Vec2i& mPosition, const ssvs::Vec2i& mSize, bool mAffectedByGravity)
		: sses::Component{mE}, world(mWorld), body(world.create(mPosition, mSize, mIsStatic)), affectedByGravity{mAffectedByGravity}, groundSensor{body, ssvs::Vec2i{body.getWidth(), 10}}
	{
		groundSensor.getSensor().addGroupsToCheck(LDGroup::Solid);

		body.setUserData(&getEntity());

		body.onDetection += [this](const DetectionInfo& mDI)
		{
			if(mDI.userData == nullptr) return;
			Entity* e(static_cast<Entity*>(mDI.userData));
			onDetection(*e);
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
