// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDDependencies.h"
#include "LDAssets.h"
#include "LDGame.h"

using namespace ld;
using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvms;

struct LDMenu
{
	GameWindow& window;
	LDAssets& assets;
	GameState gameState;
	LDGame& game;
	Menu menu;
	BitmapText txt;
	Camera camera{window, {{800 / 2 - 200, 600 / 2 - 150}, {400, 300}}};

	LDMenu(GameWindow& mGameWindow, LDAssets& mAssets, LDGame& mGame) : window(mGameWindow), assets(mAssets), game(mGame),
		txt{assets.get<BitmapFont>("limeStroked"), ""}
	{
		txt.setTracking(-3);

		gameState.onUpdate += [&](float mFrameTime){ update(mFrameTime); };
		gameState.onDraw += [&]{ draw(); };

		using k = Keyboard::Key;
		using b = Mouse::Button;
		using t = Input::Trigger::Type;
		gameState.addInput({{k::Up}},					[&](float){ menu.previous(); }, t::Once);
		gameState.addInput({{k::Down}},					[&](float){ menu.next(); }, t::Once);
		gameState.addInput({{k::Left}},					[&](float){ menu.decrease(); }, t::Once);
		gameState.addInput({{k::Right}},				[&](float){ menu.increase(); }, t::Once);
		gameState.addInput({{k::Return}, {k::Space}},	[&](float){ menu.exec(); }, t::Once);
		gameState.addInput({{k::Escape}},				[&](float){ menu.goBack(); }, t::Once);

		namespace i = ssvms::Items;
		auto& main = menu.createCategory("10corp");
		main.create<i::Single>("play", [&]{ game.newGame(); window.setGameState(game.getGameState()); });
		main.create<i::Single>("exit", [&]{ window.stop(); });

	}

	inline void update(float mFT) { }
	inline void draw() { camera.apply(); drawMenu(menu); camera.unapply(); }

	inline void render(Drawable& mDrawable) { window.draw(mDrawable); }
	inline BitmapText& renderTextImpl(const string& mStr, BitmapText& mText, Vec2f mPosition)
	{
		if(mText.getString() != mStr) mText.setString(mStr);
		mText.setPosition(mPosition); render(mText); return mText;
	}
	inline const Color& getTextColor() const { return Color::White; }
	inline BitmapText& renderText(const string& mStr, BitmapText& mText, Vec2f mPos)						{ mText.setColor(getTextColor()); return renderTextImpl(mStr, mText, mPos); }
	inline BitmapText& renderText(const string& mStr, BitmapText& mText, Vec2f mPos, const Color& mColor)	{ mText.setColor(mColor); return renderTextImpl(mStr, mText, mPos); }

	void drawMenu(const Menu& mMenu)
	{
		renderText(mMenu.getCategory().getName(), txt, {0.f, 0.f});

		float currentX{0.f}, currentY{0.f};
		auto& currentItems(mMenu.getItems());
		for(int i{0}; i < static_cast<int>(currentItems.size()); ++i)
		{
			currentY += 19;
			if(i != 0 && i % 21 == 0) { currentY = 0; currentX += 180; }
			string name, itemName{currentItems[i]->getName()};
			if(i == mMenu.getIndex()) name.append(">> ");
			name.append(itemName);

			int extraSpacing{0};
			if(itemName == "back") extraSpacing = 20;
			renderText(name, txt, {20.f + currentX, 20.f + currentY + extraSpacing}, currentItems[i]->isEnabled() ? Color::White : Color{155, 155, 155, 255});
		}
	}

};

int main()
{
	unsigned int width{VideoMode::getDesktopMode().width}, height{VideoMode::getDesktopMode().height};
	width = 800; height = 600;

	LDAssets assets;

	GameWindow gameWindow;
	gameWindow.setTitle("Ludum Dare #27");
	gameWindow.setTimer<StaticTimer>(0.5f, 0.5f);
	gameWindow.setSize(width, height);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);

	LDGame game{gameWindow, assets};
	LDMenu menuGame{gameWindow, assets, game};

	gameWindow.setGameState(menuGame.gameState);
	gameWindow.run();

	return 0;
}
