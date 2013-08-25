// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_CONFIG
#define SSVLD_CONFIG

#include "LDDependencies.h"

namespace ld
{
	struct LDConfig
	{
		bool soundEnabled;
		bool musicEnabled;

		inline void init()
		{
			soundEnabled = true;
			musicEnabled = true;
		}

		inline static LDConfig& get() { static LDConfig instance; return instance; }
	};
}

#endif
