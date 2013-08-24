// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDGame.h"
#include "LDGroups.h"
#include "LDCPhysics.h"
#include "LDCPlayer.h"

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
		world(createResolver<Retro>(), createSpatial<Grid>(1000, 1000, 3000, 500)),  debugText{assets.get<BitmapFont>("limeStroked")}, msgText{assets.get<BitmapFont>("limeStroked")},
		timerText{assets.get<BitmapFont>("limeStroked")}
	{
		// Let's initialize stuff from my game framework
		// These are delegates from SSVUtils, similar to C# delegates
		gameState.onUpdate += [this](float mFrameTime){ update(mFrameTime); };
		gameState.onDraw += [this]{ draw(); };

		// Let's make the text prettier
		debugText.setTracking(-3);
		msgText.setTracking(-3);
		msgText.setScale(2.f, 2.f);
		timerText.setTracking(-3);
		timerText.setScale(4.f, 4.f);

		// Input initialization
		using k = Keyboard::Key;
		using b = Mouse::Button;
		using t = Input::Trigger::Type;

		gameState.addInput({{k::Escape}}, [&](float){ gameWindow.stop(); });

		gameState.addInput({{k::A}}, [=](float mFrameTime){ camera.move(Vec2f{-10, 0} * mFrameTime); });
		gameState.addInput({{k::D}}, [=](float mFrameTime){ camera.move(Vec2f{10, 0} * mFrameTime); });
		gameState.addInput({{k::W}}, [=](float mFrameTime){ camera.move(Vec2f{0, -10} * mFrameTime); });
		gameState.addInput({{k::S}}, [=](float mFrameTime){ camera.move(Vec2f{0, 10} * mFrameTime); });

		add2StateInput(gameState, {{k::Z}}, inputAction);
		add2StateInput(gameState, {{k::X}}, inputJump);

		add3StateInput(gameState, {{k::Left}}, {{k::Right}}, inputX);
		add3StateInput(gameState, {{k::Up}}, {{k::Down}}, inputY);

		gameState.addInput({{k::R}}, [&](float){ newGame(); }, t::Once);

		// Debug
		gameState.addInput({{k::Num1}}, [&](float){ factory.createWall(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num2}}, [&](float){ factory.createBlock(getMousePosition(), getRnd(0, 10)); }, t::Once);
		gameState.addInput({{k::Num3}}, [&](float){ factory.createPlayer(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num4}}, [&](float){ factory.createBlock(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num5}}, [&](float){ factory.createReceiver(getMousePosition(), getRnd(0, 10)); }, t::Once);
		gameState.addInput({{k::Num6}}, [&](float){ factory.createReceiver(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Q}}, [=](float mFrameTime){ camera.zoom(pow(1.1f, mFrameTime)); });
		gameState.addInput({{k::E}}, [=](float mFrameTime){ camera.zoom(pow(0.9f, mFrameTime)); });
	}

	void LDGame::start10Secs() { if(!levelStatus.started) { levelStatus.started = true; levelStatus.timeLeft = 10.f; } }
	void LDGame::refresh10Secs() { levelStatus.timeLeft = 10.f; }

	void LDGame::showMessage(const string& mMsg, float mDuration, const Color& mColor)
	{
		currentMsgDuration = mDuration; currentMsg = "> " + mMsg;
		msgText.setString(""); msgDone = false;
		msgText.setColor(mColor);
	}

	void LDGame::newGame()
	{
		// Cleanup
		manager.clear();
		timelineManager.clear();
		levelStatus = LDLevelStatus{};

		// Level loading
		//levelOne();
		//levelTwo();
		levelThree();
	}

	void LDGame::levelOne()
	{
		levelStatus.title = "introduction - part 1";
		levelStatus.tutorial = true;

		int sX{1000}, sY{1000};
		auto put([=](int mX, int mY){ return Vec2i(sX + 3200 * mX, sY + 3200 * mY); });
		auto pW([=](int mX, int mY) -> Entity& { return factory.createWall(put(mX, mY)); });
		auto pB([=](int mX, int mY) -> Entity& { return factory.createBlock(put(mX, mY)); });
		auto pR([=](int mX, int mY) -> Entity& { return factory.createReceiver(put(mX, mY)); });
		auto pT([=](int mX, int mY) -> Entity& { return factory.createTele(put(mX, mY)); });

		string rndWorkerHash;
		for(int i{0}; i < 10; ++i) rndWorkerHash += toStr(getRnd(1, 10));

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(0, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto pCrateX(sX + 20000);
		auto& pCrate(factory.createBlock(put(5, -1)));
		auto& pReceiver(factory.createReceiver(put(10, 1)));

																																pW(19, -6);
																																pW(19, -5);
																																pW(19, -4);
																													pB(18, -3);	pW(19, -3);
																													pB(18, -2);	pW(19, -2);
																	pT(14, -1);										pB(18, -1);	pW(19, -1);
								pW(11, 0); pW(12, 0);	pW(13, 0);	pW(14, 0);	pW(15, 0);	pW(16, 0);				pW(18, 0);	pW(19, 0);
		pW(9, 1);				pW(11, 1);													pW(16, 1);	pR(17, 1);	pW(18, 1);
		pW(9, 2);	pW(10, 2);	pW(11, 2);													pW(16, 2);	pW(17, 2);	pW(18, 2);

		const auto& pCrateBody(pCrate.getComponent<LDCPhysics>().getBody());
		const auto& pReceiverBody(pReceiver.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return pBody.getPosition().x < pCrateX - 5000 && (!msgDone || !msgText.getString().empty()); });
		auto crateNotPlaced([=, &pCrateBody, &pReceiverBody]{ return getDist(pCrateBody.getPosition(), pReceiverBody.getPosition()) > 3200; });

		auto& t(timelineManager.create());
		t.append<Do>([=]{ showMessage("10corp welcomes you, worker #" + rndWorkerHash, 150); });																		t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("#" + rndWorkerHash + ", you have been selected because of your\n<rnd_quality_1>\n<rnd_quality_2>\n<rnd_quality_3>", 150); });	t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("but your real strengths, #" + rndWorkerHash + ", are your speed,\nyour agility, your dedition to work", 150); });				t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("you should know how much 10corp values speed\nand quick thinking", 150); });														t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("here are the standard protocols to follow", 150); });																			t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("1. 10corp is your home, your workplace, your life", 150, Color::Red); });														t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("2. 10corp values speed: slow workers will be terminated\nfor the good of mankind", 150, Color::Red); });							t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("3. 10corp values intelligence: inept workers will be\nterminated for the good of mankind", 150, Color::Red); });					t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("proceed to your right for the standard newcomer training", 150, Color::White); });												t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("place the 10corp standardized cratestorage in the\n10corp standardized cratereceiver to continue", -1, Color::Green); });		t.append<WaitWhile>(crateNotPlaced);
		t.append<Do>([=]{ showMessage("assigment successful\nplace remaining cratestorages and\nproceed to the 10corp standardized molecular transporter", -1, Color::Green); });
	}

	void LDGame::levelTwo()
	{
		levelStatus.title = "introduction - part 2";
		levelStatus.tutorial = false;

		int sX{1000}, sY{1000};
		auto put([=](int mX, int mY){ return Vec2i(sX + 3200 * mX, sY + 3200 * mY); });
		auto pW([=](int mX, int mY) -> Entity& { return factory.createWall(put(mX, mY)); });
		auto pB([=](int mX, int mY) -> Entity& { return factory.createBlock(put(mX, mY)); });
		auto pR([=](int mX, int mY) -> Entity& { return factory.createReceiver(put(mX, mY)); });
		auto pT([=](int mX, int mY) -> Entity& { return factory.createTele(put(mX, mY)); });

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(0, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return !levelStatus.started && (!msgDone || !msgText.getString().empty()); });

		pW(9, 1);	pB(10, 1);																pB(16, 1);
		pW(9, 2);	pW(10, 2);										pW(14, 2);				pB(16, 2);
		pW(9, 3);	pR(10, 3);							pW(13, 3);	pW(14, 3);	pW(15, 3);	pB(16, 3);	pT(17, 3);
		pW(9, 4);	pW(10, 4);	pW(11, 4);				pW(13, 4);	pW(14, 4);	pW(15, 4);	pW(16, 4);	pW(17, 4);
								pW(11, 5);	pW(12, 5);	pW(13, 5);

		auto& t(timelineManager.create());
		t.append<Do>([=]{ showMessage("welcome back, worker", 150); });																								t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("10corp does not tolerate slowness", 150, Color::Red); });																	t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("after grabbing the first cratestorage, you\nwill only have 10 seconds before\nyou will be terminated", 150, Color::Red); });	t.append<WaitWhile>(intro);
	}

	void LDGame::levelThree()
	{
		levelStatus.title = "introduction - part 3";
		levelStatus.tutorial = false;

		int sX{1000}, sY{1000};
		auto put([=](int mX, int mY){ return Vec2i(sX + 3200 * mX, sY + 3200 * mY); });
		auto pW([=](int mX, int mY) -> Entity& { return factory.createWall(put(mX, mY)); });
		auto pB([=](int mX, int mY) -> Entity& { return factory.createBlock(put(mX, mY)); });
		auto pR([=](int mX, int mY) -> Entity& { return factory.createReceiver(put(mX, mY)); });
		auto pT([=](int mX, int mY) -> Entity& { return factory.createTele(put(mX, mY)); });

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(0, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return !levelStatus.started && (!msgDone || !msgText.getString().empty()); });

		pB(9, -3);
		pB(9, -2);
		pB(9, -1);																				pW(17, 0);
		pW(9, 1);																				pW(17, 1);
		pW(9, 2);											pW(14, 2);							pW(17, 2);
		pW(9, 3);	pT(10, 3);								pW(14, 3);	pW(15, 3);	pR(16, 3);	pW(17, 3);
		pW(9, 4);	pW(10, 4);								pW(14, 4);	pW(15, 4);	pW(16, 4);	pW(17, 4);


		auto& t(timelineManager.create());
		t.append<Do>([=]{ showMessage("speed is everything, worker", 150); });																				t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("10corp does not tolerate slowness", 150, Color::Red); });															t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("do not be afraid to throw 10corp standardized cratestorages\nif that speeds up your tasks", 150, Color::Red); });	t.append<WaitWhile>(intro);
	}

	void LDGame::update(float mFrameTime)
	{
		// 10 SECONDS!
		if(levelStatus.started && !levelStatus.tutorial)
		{
			if(levelStatus.timeLeft >= 0.f)
			{
				levelStatus.timeLeft -= mFrameTime / 60.f;
				timerText.setColor(Color::Red);
				timerText.setString(toStr(levelStatus.timeLeft));
			}
			else if(manager.getEntityCount(LDGroup::Player) > 0) manager.getEntities(LDGroup::Player)[0]->destroy();
			else timerText.setString("too slow");

			auto grow = static_cast<int>(10.f - levelStatus.timeLeft) * 0.4f;
			timerText.setScale(4.f + grow, 4.f + grow);
		}
		else
		{
			timerText.setColor(Color::Blue);
			timerText.setString(levelStatus.tutorial ? "tutorial" : "safe");
			timerText.setScale(4.f, 4.f);
		}
		timerText.setPosition({0.f, gameWindow.getHeight() - timerText.getGlobalBounds().height});

		// Message control
		if(!msgDone && msgText.getString().size() < currentMsg.size())
		{
			if(msgCharTime <= 0.f)
			{
				msgText.setString(msgText.getString() + currentMsg[msgText.getString().size()]);
				msgCharTime = 2.2f;
			}
			else msgCharTime -= mFrameTime;
		}
		else if(currentMsgDuration > 0.f)
		{
			currentMsgDuration -= mFrameTime;
			clamp(currentMsgDuration, 0.f, numeric_limits<float>::max()); // TODO: lower bound clamp
		}
		else if(currentMsgDuration != -1) msgDone = true;
		if(msgDone && !msgText.getString().empty())
		{
			if(msgCharTime <= 0.f)
			{
				msgText.setString(msgText.getString().substr(0, msgText.getString().size() - 1));
				msgCharTime = 0.7f;
			}
			else msgCharTime -= mFrameTime;
		}

		timelineManager.update(mFrameTime); // TimelineManager is from SSVUtils, it handles coroutine-like timeline objects
		manager.update(mFrameTime); // Manager is from SSVEntitySystem, it handles entities and components
		world.update(mFrameTime); // World is from SSVSCollision, it handles "physics"
		updateDebugText(mFrameTime); // And debugText is just a debugging text showing FPS and other cool info

		// TODO: more concise?
		if(manager.getEntityCount(LDGroup::Player) > 0)
		{
			auto pPos(toPixels(manager.getEntities(LDGroup::Player)[0]->getComponent<LDCPhysics>().getPos()));
			panVec += manager.getEntities(LDGroup::Player)[0]->getComponent<LDCPlayer>().isFacingLeft() ? Vec2f{-1.f, 0} : Vec2f{1.f, 0};
			clamp(panVec, -100.f, 100.f);
			camera.move(-(camera.getCenter() - (pPos + panVec)) / 40.f);
		}

		if(manager.getEntityCount(LDGroup::Block) == 0) levelStatus.started = false;
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

		if(manager.getEntityCount(LDGroup::Block) == 0) s << "SAFE: NO BLOCKS" << endl;

		debugText.setString(s.str());
	}

	void LDGame::draw()
	{
		camera.apply();
		manager.draw();
		camera.unapply();
		//render(debugText);
		render(msgText);
		render(timerText);
	}
}

// I absolutely have no idea what the game design should be
// Sort of
