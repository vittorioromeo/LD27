// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDGame.h"

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvs::Utils;
using namespace ssvsc;
using namespace ssvsc::Utils;
using namespace sses;

namespace ld
{
	LDGame::LDGame(GameWindow& mGameWindow, LDAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets), factory{assets, *this, manager, world},
		world(createResolver<Retro>(), createSpatial<Grid>(1000, 1000, 3000, 500)),  debugText{assets.get<BitmapFont>("limeStroked")}
	{
		// Let's initialize stuff from my game framework
		// These are delegates from SSVUtils, similar to C# delegates
		gameState.onUpdate += [this](float mFrameTime){ update(mFrameTime); };
		gameState.onDraw += [this]{ draw(); };

		// Let's make the debug text prettier
		debugText.setTracking(-3);

		// Input initialization
		using k = Keyboard::Key;
		using b = Mouse::Button;
		using t = Input::Trigger::Type;

		gameState.addInput({{k::Escape}}, [&](float){ gameWindow.stop(); });

		gameState.addInput({{k::A}}, [=](float mFrameTime){ camera.move(Vec2f{-10, 0} * mFrameTime); });
		gameState.addInput({{k::D}}, [=](float mFrameTime){ camera.move(Vec2f{10, 0} * mFrameTime); });
		gameState.addInput({{k::W}}, [=](float mFrameTime){ camera.move(Vec2f{0, -10} * mFrameTime); });
		gameState.addInput({{k::S}}, [=](float mFrameTime){ camera.move(Vec2f{0, 10} * mFrameTime); });
		gameState.addInput({{k::Q}}, [=](float mFrameTime){ camera.zoom(pow(1.1f, mFrameTime)); });
		gameState.addInput({{k::E}}, [=](float mFrameTime){ camera.zoom(pow(0.9f, mFrameTime)); });

		add2StateInput(gameState, {{k::Z}}, inputAction);
		add2StateInput(gameState, {{k::X}}, inputJump);

		add3StateInput(gameState, {{k::Left}}, {{k::Right}}, inputX);
		add3StateInput(gameState, {{k::Up}}, {{k::Down}}, inputY);

		// Debug
		gameState.addInput({{k::Num1}}, [&](float){ factory.createWall(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num2}}, [&](float){ factory.createBlock(getMousePosition(), getRnd(0, 10)); }, t::Once);
		gameState.addInput({{k::Num3}}, [&](float){ factory.createPlayer(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num4}}, [&](float){ factory.createBlock(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num5}}, [&](float){ factory.createReceiver(getMousePosition(), getRnd(0, 10)); }, t::Once);
		gameState.addInput({{k::Num6}}, [&](float){ factory.createReceiver(getMousePosition()); }, t::Once);

		// Level debug
		for(int i{0}; i < 30; ++i) factory.createWall({i * 3200, 0});
	}

	void LDGame::update(float mFrameTime)
	{
		// Manager is from SSVEntitySystem, it handles entities and components
		manager.update(mFrameTime);

		// World is from SSVSCollision, it handles "physics"
		world.update(mFrameTime);

		// And debugText is just a debugging text showing FPS and other cool info
		updateDebugText(mFrameTime);
	}
	void LDGame::updateDebugText(float mFrameTime)
	{
		ostringstream s;
		const auto& entities(manager.getEntities());
		const auto& bodies(world.getBodies());
		std::size_t componentCount{0}, dynamicBodiesCount{0};
		for(const auto& e : entities) componentCount += e->getComponents().size();
		for(const auto& b : bodies) if(!b->isStatic()) ++dynamicBodiesCount;

		s << "FPS: "				<< gameWindow.getFPS() << endl;
		s << "FrameTime: "			<< mFrameTime << endl;
		s << "Bodies(all): "		<< bodies.size() << endl;
		s << "Bodies(static): "		<< bodies.size() - dynamicBodiesCount << endl;
		s << "Bodies(dynamic): "	<< dynamicBodiesCount << endl;
		s << "Sensors: "			<< world.getSensors().size() << endl;
		s << "Entities: "			<< entities.size() << endl;
		s << "Components: "			<< componentCount << endl;

		debugText.setString(s.str());
	}

	void LDGame::draw()
	{
		camera.apply();
		manager.draw();
		camera.unapply();
		render(debugText);
	}
}

// I absolutely have no idea what the game design should be

