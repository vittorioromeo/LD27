// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_MENU
#define SSVLD_MENU

#include "LDDependencies.h"
#include "LDAssets.h"
#include "LDConfig.h"
#include "LDGame.h"

namespace ld
{
	struct LDMenu
	{
		ssvs::GameWindow& window;
		LDAssets& assets;
		ssvs::GameState gameState;
		LDGame& game;
		ssvms::Menu menu;
		ssvs::BitmapText txt, creditsTxt;
		ssvs::Camera camera{window, 2.f};
		int level{0};

		LDMenu(ssvs::GameWindow& mGameWindow, LDAssets& mAssets, LDGame& mGame) : window(mGameWindow), assets(mAssets), game(mGame),
			txt{assets.get<ssvs::BitmapFont>("limeStroked")}, creditsTxt{assets.get<ssvs::BitmapFont>("limeStroked")}
		{
			txt.setTracking(-3);
			creditsTxt.setTracking(-3);

			creditsTxt.setString("entry for ludum dare #27\ncreated by vittorio romeo\nhttp://vittorioromeo.info\n\nmove with arrow keys\nkeep 'Z' pressed to grab blocks\npress 'X' to jump\nuse 'W/S/A/D' to pan camera\nuse 'Q/E' to zoom\npress 'R' to restart the level");
			creditsTxt.setPosition({window.getWidth() - creditsTxt.getGlobalBounds().width, window.getHeight() - creditsTxt.getGlobalBounds().height});

			gameState.onUpdate += [this](float mFT){ update(mFT); };
			gameState.onDraw += [this]{ draw(); };

			using k = sf::Keyboard::Key;
			using b = sf::Mouse::Button;
			using t = ssvs::Input::Trigger::Type;
			gameState.addInput({{k::Up}},					[this](float){ assets.playSound("blip.wav"); menu.previous(); }, t::Once);
			gameState.addInput({{k::Down}},					[this](float){ assets.playSound("blip.wav"); menu.next(); }, t::Once);
			gameState.addInput({{k::Left}},					[this](float){ assets.playSound("blip.wav"); menu.decrease(); }, t::Once);
			gameState.addInput({{k::Right}},				[this](float){ assets.playSound("blip.wav"); menu.increase(); }, t::Once);
			gameState.addInput({{k::Return}, {k::Space}},	[this](float){ assets.playSound("blip.wav"); menu.exec(); }, t::Once);

			namespace i = ssvms::Items;
			auto& main = menu.createCategory("10corp");
			main.create<i::Single>("play", [this]
			{
				game.setLevel(level); game.newGame(); window.setGameState(game.getGameState());
				assets.playMusic("mus.ogg");
			});
			main.create<i::Slider>("starting level", [this]{ return level; }, [this](int l){ level = l; }, 0, LDGame::levelCount, 1);

			main.create<i::Toggle>("sound", LDConfig::get().soundEnabled);
			main.create<i::Slider>("sound volume", [this]{ return assets.soundPlayer.getVolume(); }, [this](int v){ assets.soundPlayer.setVolume(v); }, 0, 100, 10) | [this]{ return LDConfig::get().soundEnabled; };
			main.create<i::Toggle>("music", LDConfig::get().musicEnabled);
			main.create<i::Slider>("music volume", [this]{ return assets.musicPlayer.getVolume(); }, [this](int v){ assets.musicPlayer.setVolume(v); }, 0, 100, 10) | [this]{ return LDConfig::get().musicEnabled; };

			main.create<i::Single>("exit", [this]{ window.stop(); });
		}

		inline void update(float mFT) { camera.update(mFT); menu.update(); }
		inline void draw() { camera.apply(); drawMenu(menu); camera.unapply(); render(creditsTxt); }

		inline void render(sf::Drawable& mDrawable) { window.draw(mDrawable); }
		inline ssvs::BitmapText& renderTextImpl(const std::string& mStr, ssvs::BitmapText& mText, const ssvs::Vec2f& mPosition)
		{
			if(mText.getString() != mStr) mText.setString(mStr);
			mText.setPosition(mPosition); render(mText); return mText;
		}
		inline const sf::Color& getTextColor() const { return sf::Color::White; }
		inline ssvs::BitmapText& renderText(const std::string& mStr, ssvs::BitmapText& mText, const ssvs::Vec2f& mPos)							{ mText.setColor(getTextColor()); return renderTextImpl(mStr, mText, mPos); }
		inline ssvs::BitmapText& renderText(const std::string& mStr, ssvs::BitmapText& mText, const ssvs::Vec2f& mPos, const sf::Color& mColor)	{ mText.setColor(mColor); return renderTextImpl(mStr, mText, mPos); }

		void drawMenu(const ssvms::Menu& mMenu)
		{
			renderText(mMenu.getCategory().getName(), txt, ssvs::zeroVec2f);

			float currentX{0.f}, currentY{0.f};
			auto& currentItems(mMenu.getItems());
			for(int i{0}; i < static_cast<int>(currentItems.size()); ++i)
			{
				currentY += 12;
				if(i != 0 && i % 21 == 0) { currentY = 0; currentX += 180; }
				std::string name, itemName{currentItems[i]->getName()};
				if(i == mMenu.getIdx()) name.append(">> ");
				name.append(itemName);

				int extraSpacing{0};
				if(itemName == "back") extraSpacing = 20;
				renderText(name, txt, {20.f + currentX, 20.f + currentY + extraSpacing}, currentItems[i]->isEnabled() ? sf::Color::White : sf::Color{155, 155, 155, 255});
			}
		}
	};
}

#endif
