// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

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
		auto& cPhysics(result.createComponent<LDCPhysics>(world, true, mPos, Vec2i{1600, 1600}));
		auto& cRender(result.createComponent<LDCRender>(game, cPhysics.getBody()));

		Body& body(cPhysics.getBody());
		body.addGroup(LDGroup::Solid);
		body.addGroupToCheck(LDGroup::Solid);

		Sprite s{assets.get<Texture>("worldTiles.png")};
		s.setTextureRect(assets.tilesetWorld[{1, 0}]);

		cRender.addSprite(s);
		cRender.setScaleWithBody(true);

		return result;
	}
	Entity& LDFactory::createBlock(Vec2i mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<LDCPhysics>(world, false, mPos, Vec2i{1600, 1600}));
		auto& cRender(result.createComponent<LDCRender>(game, cPhysics.getBody()));
		result.createComponent<LDCBlock>(game, cPhysics);

		Body& body(cPhysics.getBody());
		body.addGroup(LDGroup::Solid);
		body.addGroup(LDGroup::Block);
		body.addGroup(LDGroup::CanBePicked);
		body.addGroupToCheck(LDGroup::Solid);
		body.addGroupNoResolve(LDGroup::Player);
		body.addGroupNoResolve(LDGroup::BlockFloating);

		Sprite s{assets.get<Texture>("worldTiles.png")};
		s.setTextureRect(assets.tilesetWorld[{0, 0}]);
		s.setColor(Color(getRnd(0, 255), getRnd(0, 255), getRnd(0, 255), 255));

		cRender.addSprite(s);
		cRender.setScaleWithBody(true);

		return result;
	}

	Entity& LDFactory::createPlayer(Vec2i mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<LDCPhysics>(world, false, mPos, Vec2i{1000, 3200}));
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
}

