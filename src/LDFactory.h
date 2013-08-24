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

		public:
			LDFactory(LDAssets& mAssets, LDGame& mGame, sses::Manager& mManager, ssvsc::World& mWorld);

			sses::Entity& createWall(ssvs::Vec2i mPos);
			sses::Entity& createBlock(ssvs::Vec2i mPos, int mVal = -1);
			sses::Entity& createPlayer(ssvs::Vec2i mPos);
			sses::Entity& createReceiver(ssvs::Vec2i mPos, int mVal = -1);
			sses::Entity& createTele(ssvs::Vec2i mPos);
	};
}

#endif
