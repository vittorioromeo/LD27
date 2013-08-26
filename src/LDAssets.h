// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_ASSETS
#define SSVLD_ASSETS

#include "LDDependencies.h"
#include "LDConfig.h"

namespace ld
{
	class LDAssets
	{
		private:
			ssvs::AssetManager assetManager;

		public:
			ssvs::SoundPlayer soundPlayer;
			ssvs::MusicPlayer musicPlayer;
			ssvs::Tileset tilesetChar{ssvuj::as<ssvs::Tileset>(ssvuj::readFromFile("Data/Tilesets/tilesetChar.json"))};
			ssvs::Tileset tilesetWorld{ssvuj::as<ssvs::Tileset>(ssvuj::readFromFile("Data/Tilesets/tilesetWorld.json"))};

			inline LDAssets()
			{
				ssvs::loadAssetsFromJson(assetManager, "Data/", ssvuj::readFromFile("Data/assets.json"));
				assetManager.load<ssvs::BitmapFont>("limeStroked", assetManager.get<sf::Texture>("limeStroked.png"), ssvuj::as<ssvs::BitmapFontData>(ssvuj::readFromFile("Data/lime.json")));

				soundPlayer.setVolume(50);
				musicPlayer.setVolume(30);
			}

			inline ssvs::AssetManager& operator()() { return assetManager; }
			template<typename T> inline T& get(const std::string& mId) { return assetManager.get<T>(mId); }

			inline void playSound(const std::string& mName, ssvs::SoundPlayer::Mode mMode = ssvs::SoundPlayer::Mode::Overlap, float mPitch = 1.f)
			{
				if(!LDConfig::get().soundEnabled) return;
				soundPlayer.play(get<sf::SoundBuffer>(mName), mMode, mPitch);
			}
			inline void playMusic(const std::string& mName)
			{
				if(!LDConfig::get().musicEnabled) return;
				musicPlayer.play(get<sf::Music>(mName));
				musicPlayer.setLoop(true);
			}
	};
}

#endif
