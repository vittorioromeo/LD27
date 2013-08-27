// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "LDGame.h"
#include "LDMenu.h"
#include "LDGroups.h"
#include "LDCPhysics.h"
#include "LDCPlayer.h"

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
	int LDGame::levelCount{6};

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

		gameState.addInput({{k::Escape}}, [&](float){ assets.musicPlayer.stop(); gameWindow.setGameState(menuGame->gameState); });

		gameState.addInput({{k::A}}, [=](float){ camera.pan(-10, 0); });
		gameState.addInput({{k::D}}, [=](float){ camera.pan(10, 0); });
		gameState.addInput({{k::W}}, [=](float){ camera.pan(0, -10); });
		gameState.addInput({{k::S}}, [=](float){ camera.pan(0, 10); });
		gameState.addInput({{k::Q}}, [=](float){ camera.zoomOut(1.1f); });
		gameState.addInput({{k::E}}, [=](float){ camera.zoomIn(1.1f); });

		add2StateInput(gameState, {{k::Z}}, inputAction);
		add2StateInput(gameState, {{k::X}}, inputJump);

		add3StateInput(gameState, {{k::Left}}, {{k::Right}}, inputX);
		add3StateInput(gameState, {{k::Up}}, {{k::Down}}, inputY);

		gameState.addInput({{k::R}}, [&](float){ newGame(); }, t::Once);

		gameState.addInput({{k::Num1}}, [&](float){ factory.createWall(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num2}}, [&](float){ factory.createBlock(getMousePosition(), getRnd(0, 10)); }, t::Once);
		gameState.addInput({{k::Num3}}, [&](float){ factory.createPlayer(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num4}}, [&](float){ factory.createBlock(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num5}}, [&](float){ factory.createReceiver(getMousePosition(), getRnd(0, 10)); }, t::Once);
		gameState.addInput({{k::Num6}}, [&](float){ factory.createReceiver(getMousePosition()); }, t::Once);
	}

	void LDGame::start10Secs() { if(!levelStatus.started) { levelStatus.started = true; refresh10Secs(); } }
	void LDGame::refresh10Secs() { levelStatus.timeLeft = 10.f; levelStatus.secondTime = 1.f; }

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
		switch(level)
		{
			case 0: levelOne(); break;
			case 1: levelTwo(); break;
			case 2: levelThree(); break;
			case 3: levelFour(); break;
			case 4: levelFive(); break;
			case 5: levelSix(); break;
			case 6: levelSeven(); break;
		}
	}
	void LDGame::nextLevel() { level = level < levelCount ? level + 1 : 0; mustChangeLevel = true; }

	constexpr int sX{1000}; constexpr int sY{1000};
	static Vec2i put(int mX, int mY) { return Vec2i(sX + 3200 * mX, sY + 3200 * mY); }
	Entity& LDGame::pW(int mX, int mY)				{ return factory.createWall(put(mX, mY)); }
	Entity& LDGame::pB(int mX, int mY, int mVal)	{ return factory.createBlock(put(mX, mY), mVal); }
	Entity& LDGame::pR(int mX, int mY, int mVal)	{ return factory.createReceiver(put(mX, mY), mVal); }
	Entity& LDGame::pT(int mX, int mY)				{ return factory.createTele(put(mX, mY)); }

	void LDGame::levelOne()
	{
		levelStatus.title = "introduction - part 1"; levelStatus.tutorial = true;

		string rndWorkerHash; for(int i{0}; i < 10; ++i) rndWorkerHash += toStr(getRnd(1, 10));

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(1, -1)));
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
		auto crateNotPlaced([=, &pCrateBody, &pReceiverBody]{ return getDistanceEuclidean(pCrateBody.getPosition(), pReceiverBody.getPosition()) > 3200; });

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

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(1, -1)));
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
		t.append<Do>([=]{ showMessage("after grabbing the first cratestorage, you\nwill only have 10 seconds before\nyour termination", 150, Color::Red); });	t.append<WaitWhile>(intro);
	}

	void LDGame::levelThree()
	{
		levelStatus.title = "introduction - part 3";

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(1, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return (!msgDone || !msgText.getString().empty()); });

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

	void LDGame::levelFour()
	{
		levelStatus.title = "introduction - part 4";

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(1, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return (!msgDone || !msgText.getString().empty()); });

														pW(13, -1);
											pR(12,0,0);	pW(13, 0);	pR(14,0,1);
		pW(9, 1);							pW(12, 1);	pW(13, 1);	pW(14, 1);							pW(17, 1);
		pW(9, 2);							pW(12, 2);				pW(14, 2);							pW(17, 2);
		pW(9, 3);	pB(10,3,1);																pB(16,3,0);				pT(18, 3);
		pW(9, 4);	pW(10, 4);	pW(11, 4);	pW(12, 4);	pW(13, 4);	pW(14, 4);	pW(15, 4);	pW(16, 4);	pW(17, 4);	pW(18, 4);


		auto& t(timelineManager.create());
		t.append<Do>([=]{ showMessage("not every cratestorage can be automatically sorted, worker", 150); });	t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("10corp does not tolerate mistakes", 150, Color::Red); });				t.append<WaitWhile>(intro);
		t.append<Do>([=]{ showMessage("the white cratereceivers accept everything, though", 150); });			t.append<WaitWhile>(intro);
	}

	void LDGame::levelFive()
	{
		levelStatus.title = "first task";

		for(int i{0}; i < 10; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(1, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return (!msgDone || !msgText.getString().empty()); });

											pW(11, -2);
											pW(11, -1);				pB(13,-1,0);
																	pB(13,0,0);
					pW(9, 1);	pB(10, 1);				pW(12, 1);	pW(13,1);	pW(14, 1);
					pW(9, 2);	pB(10, 2);				pW(12, 2);	pR(13,2,0);	pW(14, 2);				pB(16,3,1);
					pR(9,3,1);	pB(10, 3);																pB(16,3,1);
								pW(10, 4);	pW(11, 4);	pW(12, 4);	pW(13, 4);	pW(14, 4);	pW(15, 4);	pW(16, 4);
											pT(11, 5);	pR(12, 5);
		pW(8, 6);	pW(9, 6);	pW(10, 6);	pW(11, 6);

		auto& t(timelineManager.create());
		t.append<Do>([=]{ showMessage("of course, 10corp assumes you can use your\nbrain, too", 150); });	t.append<WaitWhile>(intro);
	}

	void LDGame::levelSix()
	{
		levelStatus.title = "second task";

		for(int i{0}; i < 8; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(1, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return (!msgDone || !msgText.getString().empty()); });

											pW(11, -2);	pW(12, -2);	pW(13, -2);
														pR(12,-1,0);
					pB(9,0,1);							pW(12, 0);							pB(15,1,0);
					pB(9,1,0);							pR(12,1,1);							pB(15,1,1);
					pW(9, 3);							pW(12, 2);							pW(15, 3);


		pB(8,5,3);							pB(11,5,0);				pB(13,5,1);	pB(14,5,0);				pR(16,5,2);
		pW(8, 6);				pW(10, 6);	pW(11, 6);	pW(12, 6);	pW(13, 6);	pW(14, 6);	pW(15, 6);	pW(16, 6);
		pB(8,7,2);							pT(11, 7);	pR(12,7,3);
		pW(8, 8);	pW(9, 8);	pW(10, 8);	pW(11, 8);	pW(12, 8);


		auto& t(timelineManager.create());
		t.append<Do>([=]{ showMessage("10corp standardized cratestorages are not fragile\nuse your environment to complete your tasks", 150); });	t.append<WaitWhile>(intro);
	}

	void LDGame::levelSeven()
	{
		levelStatus.title = "third task";

		for(int i{0}; i < 8; ++i) { pW(i, 0); pW(0, -i); }
		auto& player(factory.createPlayer(put(1, -1)));
		const auto& pBody(player.getComponent<LDCPhysics>().getBody());

		auto intro([=, &pBody]{ return (!msgDone || !msgText.getString().empty()); });											pW(18, -1);
																																pW(18, 0);
																	pW(13, 1);										pB(17,2,1);	pW(18, 1);
																													pW(17, 3);	pW(18, 2);
																	pR(13,3,1);							pR(16,4,2);				pW(18, 3);
																	pW(13,4);							pR(16,5,2);				pW(18, 4);
		pR(8,5,2);	pB(9,5,0);	pB(10,5,0);							pR(13,5,0);	pB(14,5,2);	pT(15, 5);	pB(16,5,1);				pW(18, 5);
		pW(8, 6);	pW(9, 6);	pW(10, 6);	pW(11, 6);	pW(12, 6);	pW(13, 6);	pW(14, 6);	pW(15, 6);	pW(16, 6);	pW(17, 6);	pW(18, 6);


		auto& t(timelineManager.create());
		t.append<Do>([=]{ showMessage("this is your final task for today\ngood luck, worker", 150); });	t.append<WaitWhile>(intro);
	}

	void LDGame::update(float mFrameTime)
	{
		// 10 SECONDS!
		if(levelStatus.started && !levelStatus.tutorial)
		{
			if(levelStatus.secondTime >= 0.f) levelStatus.secondTime -= mFrameTime / 60.f;
			else if(levelStatus.timeLeft > 0.f)
			{
				levelStatus.secondTime = 1.f;
				assets.playSound("blip.wav", SoundPlayer::Mode::Overlap, levelStatus.timeLeft - 3.f);
			}

			if(levelStatus.timeLeft >= 0.f)
			{
				levelStatus.timeLeft -= mFrameTime / 60.f;
				timerText.setColor(Color::Red);
				timerText.setString(toStr(levelStatus.timeLeft));
			}
			else if(manager.getEntityCount(LDGroup::Player) > 0)
			{
				manager.getEntities(LDGroup::Player)[0]->destroy();
				assets.playSound("death.wav");
			}
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
			clampMin(currentMsgDuration, 0.f);
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
			cClamp(panVec, -100.f, 100.f);
			camera.pan(-(camera.getCenter() - (pPos + panVec)) / 40.f);
		}

		if(manager.getEntityCount(LDGroup::Block) == 0) levelStatus.started = false;

		if(mustChangeLevel) { mustChangeLevel = false; newGame(); }

		camera.update(mFrameTime);
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
		camera.apply<int>();
		manager.draw();
		camera.unapply();
		//render(debugText);
		render(msgText);
		render(timerText);
	}
}
