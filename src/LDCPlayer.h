// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_COMPONENTS_PLAYER
#define SSVLD_COMPONENTS_PLAYER

#include "LDDependencies.h"
#include "LDGroups.h"

namespace ld
{
	inline static int getDist(int x1, int y1, int x2, int y2)
	{
		int diffx = x1 - x2;
		int diffy = y1 - y2;
		int diffx_sqr = std::pow(diffx, 2);
		int diffy_sqr = std::pow(diffy, 2);
		return std::sqrt(diffx_sqr + diffy_sqr);
	}
	inline static int getDist(ssvs::Vec2i mA, ssvs::Vec2i mB) { return getDist(mA.x, mA.y, mB.x, mB.y); }

	class LDCBlock : public sses::Component
	{
		private:
			LDGame& game;
			LDCPhysics& cPhysics;
			ssvsc::Body& body;
			LDCPhysics* parent{nullptr};
			ssvs::Vec2i offset{0, 0};
			int val{ssvu::getRnd(0, 99)};
			ssvs::BitmapText text{game.getAssets().get<ssvs::BitmapFont>("limeStroked"), ssvu::toStr(val)};

		public:
			LDCBlock(LDGame& mGame, LDCPhysics& mCPhysics) : game(mGame), cPhysics(mCPhysics), body(cPhysics.getBody())
			{
				text.setTracking(-3);
			}

			inline void update(float mFrameTime) override
			{
				text.setPosition(toPixels(body.getShape().getVertexNW<int>()));

				if(!cPhysics.isInAir())

				if(parent == nullptr) return;
				ssvs::Vec2f v{(parent->getBody().getPosition() + offset) - body.getPosition()};
				if(getDist(parent->getBody().getPosition(), body.getPosition()) > 1500) dropped();
				else body.setVelocity(v);
			}
			inline void draw() override
			{
				game.render(text);
			}

			inline void pickedUp(LDCPhysics& mParent)
			{
				parent = &mParent;
				body.addGroupNoResolve(LDGroup::Block);
				body.addGroup(LDGroup::BlockFloating);
			}
			inline void dropped()
			{
				parent = nullptr;
				body.delGroupNoResolve(LDGroup::Block);
				body.delGroup(LDGroup::BlockFloating);
			}
			inline void setOffset(ssvs::Vec2i mOffset) { offset = mOffset; }
			inline bool hasParent() { return parent != nullptr; }
	};

	class LDCPlayer : public sses::Component
	{
		public:
			enum class Action{Standing, Walking, Jumping, Falling};

		private:
			LDGame& game;
			LDCPhysics& cPhysics;
			ssvsc::Body& body;
			Action action;
			bool facingLeft{false}, jumpReady{false};
			float walkSpeed{100.f}, jumpSpeed{420.f};

			LDCBlock* currentBlock{nullptr};

		public:
			LDCPlayer(LDGame& mGame, LDCPhysics& mCPhysics) : game(mGame), cPhysics(mCPhysics), body(cPhysics.getBody()) { }

			void init() override
			{
				body.onPreUpdate += [&]{ jumpReady = false; };
				body.onPostUpdate += [&]{ };
				body.onDetection += [&](const ssvsc::DetectionInfo& mDI)
				{
					if(hasBlock() || !mDI.body.hasGroup(LDGroup::Block)) return;
					if(!game.getIAction()) return;
					auto& entity(*static_cast<sses::Entity*>(mDI.userData));
					auto& block(entity.getComponent<LDCBlock>());

					block.pickedUp(cPhysics);
					currentBlock = &block;
				};
				cPhysics.onResolution += [&](ssvs::Vec2i mMinIntersection) { if(mMinIntersection.y < 0) jumpReady = true; };
			}
			void update(float) override
			{
				if(game.getIX() == 0) move(0);
				else if(game.getIX() == -1) move(-1);
				else if(game.getIX() == 1) move(1);

				if(game.getIJump() == 1) jump();

				const auto& velocity(body.getVelocity());

				if(velocity.x > 0) facingLeft = false;
				else if(velocity.x < 0) facingLeft = true;

				if(!cPhysics.isInAir())
				{
					if(velocity.x == 0) action = Action::Standing;
					else if(abs(velocity.x) >= walkSpeed) action = Action::Walking;
				}
				else
				{
					if(velocity.y > 0) action = Action::Falling;
					else if(velocity.y < 0) action = Action::Jumping;
				}

				if(hasBlock())
				{
					if(!game.getIAction())
					{
						currentBlock->dropped();
						currentBlock = nullptr;
						return;
					}
					currentBlock->setOffset(ssvs::Vec2i{facingLeft ? -1000 : 1000, -600});
					if(!currentBlock->hasParent()) currentBlock = nullptr;
				}
			}

			inline void move(int mDirection)	{ body.setVelocityX(walkSpeed * mDirection); }
			inline void jump()					{ if(!cPhysics.isInAir() && jumpReady) body.setVelocityY(-jumpSpeed); }

			inline Action getAction()		{ return action; }
			inline bool isJumpReady()		{ return jumpReady; }
			inline bool isFacingLeft()		{ return facingLeft; }
			inline bool hasBlock()			{ return currentBlock != nullptr; }
	};
}

#endif
