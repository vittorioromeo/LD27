// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_UTILS
#define SSVLD_UTILS

#include "LDDependencies.h"

namespace ld
{
	class LDGame;

	template<typename T> inline constexpr float toPixels(T mValue)			{ return mValue / 100; }
	template<typename T> inline constexpr int toCoords(T mValue)			{ return mValue * 100; }
	template<typename T> inline ssvs::Vec2f toPixels(ssvs::Vec2<T> mValue)	{ return {toPixels(mValue.x), toPixels(mValue.y)}; }
	template<typename T> inline ssvs::Vec2i toCoords(ssvs::Vec2<T> mValue)	{ return {toCoords(mValue.x), toCoords(mValue.y)}; }

	inline static int getDist(int x1, int y1, int x2, int y2) { return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2)); }
	inline static int getDist(ssvs::Vec2i mA, ssvs::Vec2i mB) { return getDist(mA.x, mA.y, mB.x, mB.y); }
}

#endif
