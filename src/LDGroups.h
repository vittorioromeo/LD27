// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_GROUPS
#define SSVLD_GROUPS

#include "LDDependencies.h"

namespace ld
{
	enum LDGroup : unsigned int { Solid = 0, Block, CanBePicked, Player, BlockFloating, Sensor };
}

#endif
