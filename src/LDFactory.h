// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_FACTORY
#define SSVLD_FACTORY

#include "LDDependencies.h"

namespace ld
{
	class LDAssets;
	class LDGame;

	class LDFactory
	{
		private:
			std::unordered_map<int, sf::Color> colorMap;

			LDAssets& assets;
			LDGame& game;
			sses::Manager& manager;
			ssvsc::World& world;

			sses::Entity& createBlockBase(const ssvs::Vec2i& mPos, const ssvs::Vec2i& mSize, int mVal = -1);

		public:
			LDFactory(LDAssets& mAssets, LDGame& mGame, sses::Manager& mManager, ssvsc::World& mWorld);

			sses::Entity& createWall(const ssvs::Vec2i& mPos);
			sses::Entity& createBlock(const ssvs::Vec2i& mPos, int mVal = -1);
			sses::Entity& createBlockBig(const ssvs::Vec2i& mPos, int mVal = -1);
			sses::Entity& createBlockBall(const ssvs::Vec2i& mPos, int mVal = -1);
			sses::Entity& createBlockRubberH(const ssvs::Vec2i& mPos, int mVal = -1);
			sses::Entity& createBlockRubberV(const ssvs::Vec2i& mPos, int mVal = -1);
			sses::Entity& createPlayer(const ssvs::Vec2i& mPos);
			sses::Entity& createReceiver(const ssvs::Vec2i& mPos, int mVal = -1);
			sses::Entity& createTele(const ssvs::Vec2i& mPos);
			sses::Entity& createLift(const ssvs::Vec2i& mPos, const ssvs::Vec2f& mVel);
	};
}

#endif
