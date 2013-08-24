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
			LDAssets& assets;
			LDGame& game;
			sses::Manager& manager;
			ssvsc::World& world;

		public:
			LDFactory(LDAssets& mAssets, LDGame& mGame, sses::Manager& mManager, ssvsc::World& mWorld);
	};
}

#endif
