// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_ASSETS
#define SSVLD_ASSETS

#include "LDDependencies.h"

namespace ld
{
	class LDAssets
	{
		private:
			ssvs::AssetManager assetManager;

		public:
			inline LDAssets()
			{
				ssvs::Utils::loadAssetsFromJson(assetManager, "Data/", ssvuj::readFromFile("Data/assets.json"));
				assetManager.load<ssvs::BitmapFont>("limeStroked", assetManager.get<sf::Texture>("limeStroked.png"), ssvuj::as<ssvs::BitmapFontData>(ssvuj::readFromFile("Data/lime.json")));
			}

			inline ssvs::AssetManager& operator()() { return assetManager; }
			template<typename T> inline T& get(const std::string& mId) { return assetManager.get<T>(mId); }
	};
}

#endif
