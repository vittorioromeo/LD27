// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <unordered_map>

#include "LDFactory.h"
#include "LDGroups.h"

#include "LDCPhysics.h"
#include "LDCRender.h"
#include "LDCPlayer.h"
#include "LDCPlayerAnimation.h"

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
	LDFactory::LDFactory(LDAssets& mAssets, LDGame& mGame, Manager& mManager, World& mWorld) : assets(mAssets), game(mGame), manager(mManager), world(mWorld) { }

	Entity& LDFactory::createWall(Vec2i mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<LDCPhysics>(world, true, mPos, Vec2i{3200, 3200}));
		auto& cRender(result.createComponent<LDCRender>(game, cPhysics.getBody()));

		Body& body(cPhysics.getBody());
		body.addGroup(LDGroup::Solid);
		body.addGroupToCheck(LDGroup::Solid);

		Sprite s{assets.get<Texture>("worldTiles.png")};
		if(getRnd(0, 100) < 75) s.setTextureRect(assets.tilesetWorld[{1, 0}]);
		else s.setTextureRect(assets.tilesetWorld[Vec2u(3 + getRnd(0, 2), 0)]);

		cRender.addSprite(s);
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& LDFactory::createBlock(Vec2i mPos, int mVal)
	{
		auto& result(manager.createEntity()); result.addGroup(LDGroup::Block);
		auto& cPhysics(result.createComponent<LDCPhysics>(world, false, mPos, Vec2i{1600, 1600}));
		auto& cRender(result.createComponent<LDCRender>(game, cPhysics.getBody()));
		result.createComponent<LDCBlock>(mVal, game, cPhysics);

		Body& body(cPhysics.getBody());
		body.addGroup(LDGroup::Solid);
		body.addGroup(LDGroup::Block);
		body.addGroup(LDGroup::CanBePicked);
		body.addGroupToCheck(LDGroup::Solid);
		//body.addGroupNoResolve(LDGroup::Player);
		body.addGroupNoResolve(LDGroup::BlockFloating);
		body.setRestitutionX(-0.2f);
		body.setRestitutionY(-0.2f);

		Sprite s{assets.get<Texture>("worldTiles.png")};
		s.setTextureRect(assets.tilesetWorld[{0, 0}]);

		if(mVal > -1)
		{
			if(colorMap.find(mVal) == colorMap.end()) colorMap[mVal] = Color(getRnd(0, 255), getRnd(0, 255), getRnd(0, 255), 255);
			s.setColor(colorMap[mVal]);
		}

		cRender.addSprite(s);
		return result;
	}

	Entity& LDFactory::createPlayer(Vec2i mPos)
	{
		auto& result(manager.createEntity()); result.addGroup(LDGroup::Player);
		auto& cPhysics(result.createComponent<LDCPhysics>(world, false, mPos, Vec2i{800, 2700}));
		auto& cRender(result.createComponent<LDCRender>(game, cPhysics.getBody()));
		auto& cPlayer(result.createComponent<LDCPlayer>(game, cPhysics));
		result.createComponent<LDCPlayerAnimation>(assets.tilesetChar, cRender, cPlayer);

		Body& body(cPhysics.getBody());
		body.addGroup(LDGroup::Solid);
		body.addGroup(LDGroup::Player);
		body.addGroupToCheck(LDGroup::Solid);
		body.addGroupNoResolve(LDGroup::BlockFloating);

		cRender.addSprite(Sprite{assets.get<Texture>("charTiles.png")});
		cRender.addSprite(Sprite{assets.get<Texture>("charTiles.png")});
		cRender.setScaleWithBody(false);

		result.setDrawPriority(-1000);
		return result;
	}

	Entity& LDFactory::createReceiver(Vec2i mPos, int mVal)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<LDCPhysics>(world, false, mPos, Vec2i{1600, 1600}));
		cPhysics.setAffectedByGravity(false);
		auto& cRender(result.createComponent<LDCRender>(game, cPhysics.getBody()));
		result.createComponent<LDCBlock>(mVal, game, cPhysics);

		Body& body(cPhysics.getBody());
		body.addGroupToCheck(LDGroup::Block);
		body.setResolve(false);

		body.onDetection += [this, mVal](const DetectionInfo& mDI)
		{
			if(!mDI.body.hasGroup(LDGroup::Block)) return;
			auto& entity(*static_cast<Entity*>(mDI.userData));

			if(mVal == -1 || (mVal == entity.getComponent<LDCBlock>().getVal()))
			{
				game.getAssets().playSound("recv.wav", SoundPlayer::Mode::Override);
				entity.destroy();
				this->game.refresh10Secs();
			}
		};

		Sprite s{assets.get<Texture>("worldTiles.png")};
		s.setTextureRect(assets.tilesetWorld[{5, 0}]);

		if(mVal > -1)
		{
			if(colorMap.find(mVal) == colorMap.end()) colorMap[mVal] = Color(getRnd(0, 255), getRnd(0, 255), getRnd(0, 255), 255);
			s.setColor(colorMap[mVal]);
		}

		cRender.addSprite(s);
		return result;
	}

	Entity& LDFactory::createTele(Vec2i mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<LDCPhysics>(world, false, mPos, Vec2i{1600, 100}));
		cPhysics.setAffectedByGravity(false);
		auto& cRender(result.createComponent<LDCRender>(game, cPhysics.getBody()));

		Body& body(cPhysics.getBody());
		body.addGroupToCheck(LDGroup::Player);
		body.setResolve(false);

		body.onDetection += [this](const DetectionInfo& mDI)
		{
			if(!mDI.body.hasGroup(LDGroup::Player)) return;

			if(manager.getEntityCount(LDGroup::Block) == 0)
			{
				game.nextLevel();
				game.getAssets().playSound("tele.wav", SoundPlayer::Mode::Override);
			}
		};

		Sprite s{assets.get<Texture>("worldTiles.png")};
		s.setTextureRect(assets.tilesetWorld[{6, 0}]);

		cRender.addSprite(s);
		return result;
	}
}

