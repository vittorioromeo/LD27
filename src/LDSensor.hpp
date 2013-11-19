// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_SENSOR
#define SSVLD_SENSOR

#include "LDDependencies.hpp"
#include "LDGroups.hpp"

namespace ld
{
	class LDGame;
	class LDCPhysics;

	class LDSensor
	{
		private:
			Body& parent;
			ssvs::Vec2i position;
			Sensor& sensor;
			bool active{false};

		public:
			ssvu::Delegate<void(sses::Entity&)> onDetection;

			LDSensor(Body& mParent, const ssvs::Vec2i& mSize) : parent(mParent), position(parent.getPosition()), sensor(parent.getWorld().createSensor(position, mSize))
			{
				sensor.addGroups(LDGroup::GSensor);

				sensor.onPreUpdate += [this]{ active = false; sensor.setPosition(position); };
				sensor.onDetection += [this](const DetectionInfo& mDI)
				{
					if(&mDI.body == &parent) return;
					active = true;

					if(mDI.userData == nullptr) return;
					auto& entity(*static_cast<sses::Entity*>(mDI.userData));
					onDetection(entity);
				};
			}
			~LDSensor() { sensor.destroy(); }

			void setPosition(const ssvs::Vec2i& mPos) { position = mPos; }

			inline Sensor& getSensor()		{ return sensor; }
			inline bool isActive() const	{ return active; }
	};
}

#endif
