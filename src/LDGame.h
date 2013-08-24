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
	struct LDLevelStatus
	{
		std::string title{"unnamed level"};
		bool tutorial{false};

		bool started{false};
		float timeLeft{0.f};
	};

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
			ssvu::TimelineManager timelineManager;
			LDLevelStatus levelStatus;

			ssvs::BitmapText msgText;
			std::string currentMsg;
			float currentMsgDuration{0.f};
			float msgCharTime{0.f};
			bool msgDone{false};

			ssvs::BitmapText timerText;
			ssvs::Vec2f panVec;
			bool inputAction{false}, inputJump{false};
			int inputX{0}, inputY{0};

			bool mustChangeLevel{false};
			int level{0};

		public:
			LDGame(ssvs::GameWindow& mGameWindow, LDAssets& mAssets);

			void start10Secs();
			void refresh10Secs();

			void showMessage(const std::string& mMsg, float mDuration, const sf::Color& mColor = sf::Color::White);

			void newGame();
			void nextLevel();

			void levelOne();
			void levelTwo();
			void levelThree();
			void levelFour();

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

			inline bool getIAction()					{ return inputAction; }
			inline bool getIJump()						{ return inputJump; }
			inline int getIX()							{ return inputX; }
			inline int getIY()							{ return inputY; }
	};
}

#endif
