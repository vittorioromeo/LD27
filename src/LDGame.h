// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_GAME
#define SSVLD_GAME

#include "LDDependencies.h"
#include "LDAssets.h"
#include "LDFactory.h"
#include "LDUtils.h"

namespace ld
{
	class LDGame
	{
		private:
			ssvs::GameWindow& gameWindow;
			LDAssets& assets;
			ssvs::Camera camera{gameWindow, {{800 / 2 - 200, 600 / 2 - 150}, {400, 300}}};
			LDFactory factory;
			ssvs::GameState gameState;
			ssvsc::World world;
			sses::Manager manager;
			ssvs::BitmapText debugText;

		public:
			LDGame(ssvs::GameWindow& mGameWindow, LDAssets& mAssets);

			void update(float mFrameTime);
			void updateDebugText(float mFrameTime);
			void draw();
			inline void render(const sf::Drawable& mDrawable) { gameWindow.draw(mDrawable); }

			inline ssvs::Vec2i getMousePosition()		{ return toCoords(camera.getMousePosition()); }
			inline ssvs::GameWindow& getGameWindow()	{ return gameWindow; }
			inline LDAssets& getAssets()				{ return assets; }
			inline LDFactory& getFactory()				{ return factory; }
			inline ssvs::GameState& getGameState()		{ return gameState; }
			inline ssvsc::World& getWorld()				{ return world; }
			inline sses::Manager& getManager()			{ return manager; }
	};
}

#endif
