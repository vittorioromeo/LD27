// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_COMPONENTS_PLAYER
#define SSVLD_COMPONENTS_PLAYER

#include "LDDependencies.h"
#include "LDGroups.h"
#include "LDSensor.h"

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
			int val;
			LDGame& game;
			LDCPhysics& cPhysics;
			ssvsc::Body& body;
			LDCPhysics* parent{nullptr};
			ssvs::Vec2i offset{0, 0};
			ssvs::BitmapText text;

		public:
			ssvu::Delegate<void()> onDestroy;

			LDCBlock(int mVal, LDGame& mGame, LDCPhysics& mCPhysics) : val(mVal), game(mGame), cPhysics(mCPhysics), body(cPhysics.getBody()),
				text{game.getAssets().get<ssvs::BitmapFont>("limeStroked"), ssvu::toStr(val)}
			{
				text.setTracking(-3);
			}
			~LDCBlock() { onDestroy(); }

			inline void update(float mFrameTime) override
			{
				text.setPosition(toPixels(body.getShape().getVertexNW<int>()) + ssvs::Vec2f{3, 3});

				if(!cPhysics.isInAir())
				{
					// TODO: add getLowerClamped and getUpperClamped methods to ssv framework
					body.setVelocity(ssvs::Utils::getClamped(body.getVelocity() * 0.9f, -2000.f, 2000.f));
					if(parent == nullptr) body.delGroupNoResolve(LDGroup::Player);
				}

				if(parent == nullptr) return;
				ssvs::Vec2f v{(parent->getBody().getPosition() + offset) - body.getPosition()};
				if(getDist(parent->getBody().getPosition(), body.getPosition()) > 1700)
				{
					dropped();
				}
				else body.setVelocity(v);
			}
			inline void draw() override
			{
				if(val > -1) game.render(text);
			}

			inline void pickedUp(LDCPhysics& mParent)
			{
				parent = &mParent;
				body.addGroupNoResolve(LDGroup::Block);
				body.addGroup(LDGroup::BlockFloating);
				body.addGroupNoResolve(LDGroup::Player);
			}
			inline void dropped(float mHBoost = 1.f, float mVBoost = 1.f)
			{
				parent = nullptr;
				body.setVelocityX(ssvu::getClamped(body.getVelocity().x * mHBoost, -1000.f, 1000.f));
				body.setVelocityY(ssvu::getClamped(body.getVelocity().y * mVBoost, -1000.f, 1000.f));
				body.delGroupNoResolve(LDGroup::Block);
				body.delGroup(LDGroup::BlockFloating);
			}
			inline void setOffset(ssvs::Vec2i mOffset) { offset = mOffset; }
			inline bool hasParent() { return parent != nullptr; }
			inline int getVal() { return val; }
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
			float walkSpeed{150.f}, jumpSpeed{520.f};
			bool wasFacingLeft{false}; float lastTurn{0.f};
			float lastJump{0.f};

			LDSensor blockSensor{cPhysics, ssvs::Vec2i{10, 2400}};
			LDCBlock* currentBlock{nullptr};
			ssvsc::Body* lastBlock{nullptr};
			float lastBlockTimer{0.f};

		public:
			LDCPlayer(LDGame& mGame, LDCPhysics& mCPhysics) : game(mGame), cPhysics(mCPhysics), body(cPhysics.getBody()) { }

			void init() override
			{
				blockSensor.getSensor().addGroupToCheck(LDGroup::Block);

				blockSensor.onDetection += [&](sses::Entity& mE)
				{
					if(hasBlock() || !game.getIAction()) return;
					auto& block(mE.getComponent<LDCBlock>());
					block.pickedUp(cPhysics);
					if(currentBlock != nullptr) currentBlock->onDestroy.clear();
					currentBlock = &block;
					currentBlock->onDestroy += [&]{ currentBlock = nullptr; };
					lastBlock = &mE.getComponent<LDCPhysics>().getBody();
				};

				body.onPreUpdate += [&]{ jumpReady = false; };
				body.onPostUpdate += [&]{ };
				body.onDetection += [&](const ssvsc::DetectionInfo& mDI){ };
				body.onResolution += [&](const ssvsc::ResolutionInfo& mRI)
				{
					if(lastBlock == nullptr || &mRI.body != lastBlock || lastBlockTimer <= 0) return;

					// Ignore resolution against last picked block for some time
					mRI.noResolvePosition = mRI.noResolveVelocity = true;
				};

				cPhysics.onResolution += [&](ssvs::Vec2i mMinIntersection) { if(mMinIntersection.y < 0) jumpReady = true; };
			}
			void update(float mFrameTime) override
			{
				wasFacingLeft = facingLeft;

				ssvs::Vec2i offset{facingLeft ? -1000 : 1000, -600};
				blockSensor.setPosition(body.getPosition() + ssvs::Vec2i{offset.x / 2, 300});

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

				if(lastTurn <= 0.f)
				{
					if(facingLeft && !wasFacingLeft) lastTurn = 10.f;
				}
				else lastTurn -= mFrameTime;

				if(lastJump > 0.f) lastJump -= mFrameTime;

				if(hasBlock())
				{
					lastBlockTimer = 15.f;

					if(!game.getIAction())
					{
						currentBlock->dropped(((lastTurn > 0.f) ? lastTurn * 0.5f : 1.f), ((lastJump > 0.f) ? lastJump * 0.5f : 1.f));
						currentBlock = nullptr;
						return;
					}
					currentBlock->setOffset(offset);
					if(!currentBlock->hasParent()) currentBlock = nullptr;
				}
				else if(lastBlockTimer > 0) lastBlockTimer -= mFrameTime;
			}

			inline void move(int mDirection)	{ body.setVelocityX(walkSpeed * mDirection); }
			inline void jump()					{ if(!cPhysics.isInAir() && jumpReady) { body.setVelocityY(-jumpSpeed); lastJump = 10.f; } }

			inline Action getAction()		{ return action; }
			inline bool isJumpReady()		{ return jumpReady; }
			inline bool isFacingLeft()		{ return facingLeft; }
			inline bool hasBlock()			{ return currentBlock != nullptr; }
	};
}

#endif
