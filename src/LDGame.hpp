// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_GAME
#define SSVLD_GAME

#include "LDDependencies.hpp"
#include "LDAssets.hpp"
#include "LDFactory.hpp"
#include "LDUtils.hpp"

namespace ld
{
    struct LDMenu;

    struct LDLevelStatus
    {
        std::string title{"unnamed level"};
        bool tutorial{false}, started{false};
        Ticker timer{60.f};
    };

    class LDGame
    {
    private:
        ssvs::GameWindow& gameWindow;
        LDAssets& assets;
        ssvs::Camera camera{gameWindow, 2.f};
        LDFactory factory;
        ssvs::GameState gameState;
        World world;
        sses::Manager manager;
        ssvs::BitmapText debugText;
        ssvu::TimelineManager timelineManager;
        LDLevelStatus levelStatus;
        LDMenu* menuGame{nullptr};

        ssvs::BitmapText msgText;
        std::string currentMsg;
        Ticker msgCharTimer{2.f}, msgTimer{0.f, false};

        ssvs::BitmapText timerText;
        ssvs::Vec2f panVec;
        bool inputAction{false}, inputJump{false};
        int inputX{0}, inputY{0};

        bool mustChangeLevel{false};
        int level{0};

    public:
        static int levelCount;

        LDGame(ssvs::GameWindow& mGameWindow, LDAssets& mAssets);

        void start10Secs();
        void refresh10Secs();

        void showMessage(const std::string& mMsg, FT mDuration,
            const sf::Color& mColor = sf::Color::White);

        void newGame();
        void nextLevel();

        sses::Entity& pW(int mX, int mY);
        sses::Entity& pB(int mX, int mY, int mVal = -1);
        sses::Entity& pR(int mX, int mY, int mVal = -1);
        sses::Entity& pT(int mX, int mY);

        void levelOne();
        void levelTwo();
        void levelThree();
        void levelFour();
        void levelFive();
        void levelSix();
        void levelSeven();

        inline void setMenuGame(LDMenu& mMG) { menuGame = &mMG; }
        inline void setLevel(int mLevel) { level = mLevel; }

        void update(FT mFT);
        void updateDebugText(FT mFT);
        void draw();
        inline void render(const sf::Drawable& mDrawable)
        {
            gameWindow.draw(mDrawable);
        }

        inline ssvs::Vec2i getMousePosition() const
        {
            return toCoords(camera.getMousePosition());
        }
        inline ssvs::GameWindow& getGameWindow() { return gameWindow; }
        inline LDAssets& getAssets() { return assets; }
        inline LDFactory& getFactory() { return factory; }
        inline ssvs::GameState& getGameState() { return gameState; }
        inline World& getWorld() { return world; }
        inline sses::Manager& getManager() { return manager; }

        inline bool getIAction() const { return inputAction; }
        inline bool getIJump() const { return inputJump; }
        inline int getIX() const { return inputX; }
        inline int getIY() const { return inputY; }
    };
}

#endif
