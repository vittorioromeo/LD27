// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDFactory.h"

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvsc;
using namespace ssvsc::Utils;
using namespace sses;

namespace ld
{
	LDFactory::LDFactory(LDAssets& mAssets, LDGame& mGame, Manager& mManager, World& mWorld) : assets(mAssets), game(mGame), manager(mManager), world(mWorld) { }
}

