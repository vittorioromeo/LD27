// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDDependencies.h"
#include "LDAssets.h"
#include "LDConfig.h"
#include "LDMenu.h"
#include "LDGame.h"

using namespace ld;
using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvms;

int main()
{
	unsigned int width{VideoMode::getDesktopMode().width}, height{VideoMode::getDesktopMode().height};
	width = 800; height = 600;

	LDAssets assets;

	GameWindow gameWindow;
	gameWindow.setTitle("10corp - LD27 - by vittorio romeo");
	gameWindow.setTimer<StaticTimer>(0.5f, 0.5f);
	gameWindow.setSize(width, height);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);

	LDGame game{gameWindow, assets};
	LDMenu menuGame{gameWindow, assets, game};

	game.setMenuGame(menuGame);
	gameWindow.setGameState(menuGame.gameState);
	gameWindow.run();

	return 0;
}
