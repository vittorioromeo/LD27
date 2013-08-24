// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDGame.h"

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvsc;
using namespace ssvsc::Utils;
using namespace sses;

namespace ld
{
	LDGame::LDGame(GameWindow& mGameWindow, LDAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets), factory{assets, *this, manager, world},
		world(createResolver<Retro>(), createSpatial<Grid>(1000, 1000, 1000, 500)),  debugText{assets.get<BitmapFont>("limeStroked")}
	{
		// Let's initialize stuff from my game framework
		// These are delegates from SSVUtils, similar to C# delegates
		gameState.onUpdate += [this](float mFrameTime){ update(mFrameTime); };
		gameState.onDraw += [this]{ draw(); };

		// Let's make the debug text prettier
		debugText.setTracking(-3);
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

