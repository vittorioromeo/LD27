// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_COMPONENTS_PLAYER
#define SSVLD_COMPONENTS_PLAYER

#include "LDDependencies.h"
#include "LDGroups.h"
#include "LDSensor.h"
#include "LDUtils.h"

namespace ld
{
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
				body.onResolution += [&](const ssvsc::ResolutionInfo& mRI)
				{
					if(body.hasGroup(LDGroup::BlockFloating)) return;

					if((std::abs(body.getVelocity().x) > 400 && mRI.resolution.x != 0) ||
					   (std::abs(body.getVelocity().y) > 400 && mRI.resolution.y != 0))
						game.getAssets().playSound("bounce.wav");
				};
			}
			~LDCBlock() { onDestroy(); } // TODO: find a way to detect dead entities after LD27

			inline void update(float mFrameTime) override
			{
				text.setPosition(toPixels(body.getShape().getVertexNW<int>()) + ssvs::Vec2f{3, 3});

				if(!cPhysics.isInAir())
				{
					body.setVelocityX(ssvu::getClampedMin(body.getVelocity().x * 0.9f, 0.f));
					if(parent == nullptr) body.delGroupNoResolve(LDGroup::Player);
				}

				if(parent == nullptr) return;
				ssvs::Vec2f v{(parent->getBody().getPosition() + offset) - body.getPosition()};
				if(ssvs::Utils::getDistanceEuclidean(parent->getBody().getPosition(), body.getPosition()) > 1700) dropped();
				else body.setVelocity(v);
			}
			inline void draw() override { if(val > -1) game.render(text); }

			inline void pickedUp(LDCPhysics& mParent)
			{
				game.start10Secs();
				parent = &mParent;
				body.addGroup(LDGroup::BlockFloating);
				body.addGroupNoResolve(LDGroup::Player);
			}
			inline void dropped(float mHBoost = 1.f, float mVBoost = 1.f)
			{
				parent = nullptr;
				auto newVel(body.getVelocity()); newVel.x *= mHBoost; newVel.y *= mVBoost;
				body.setVelocity(ssvs::Utils::getCClamped(newVel, -1000.f, 1000.f));
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
			bool wasFacingLeft{false}; float lastTurn{0.f}; bool wasFacingRight{false};
			float lastJump{0.f}, stepTime{0.f};

			LDSensor blockSensor{cPhysics, ssvs::Vec2i{10, 2400}};
			LDCBlock* currentBlock{nullptr};
			ssvsc::Body* lastBlock{nullptr};
			float lastBlockTimer{0.f};

		public:
			LDCPlayer(LDGame& mGame, LDCPhysics& mCPhysics) : game(mGame), cPhysics(mCPhysics), body(cPhysics.getBody()) { }
			~LDCPlayer()
			{
				if(currentBlock == nullptr) return;

				currentBlock->onDestroy.clear();
				currentBlock->dropped();
			}

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

					game.getAssets().playSound("pick.wav", ssvs::SoundPlayer::Mode::Abort);
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
				wasFacingRight = !facingLeft;

				ssvs::Vec2i offset{facingLeft ? -1000 : 1000, -600};
				blockSensor.setPosition(body.getPosition() + ssvs::Vec2i{offset.x / 2, 300});

				if(game.getIX() == 0) move(0, mFrameTime);
				else if(game.getIX() == -1) move(-1, mFrameTime);
				else if(game.getIX() == 1) move(1, mFrameTime);

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

				if(facingLeft && !wasFacingLeft) lastTurn = 20.f;
				if(!facingLeft && !wasFacingRight) lastTurn = 20.f;

				if(lastTurn > 0.f) lastTurn -= mFrameTime;
				if(lastJump > 0.f) lastJump -= mFrameTime;

				if(hasBlock())
				{
					lastBlockTimer = 15.f;

					if(!game.getIAction())
					{
						game.getAssets().playSound("drop.wav", ssvs::SoundPlayer::Mode::Abort);
						currentBlock->dropped(((lastTurn > 0.f) ? lastTurn * 0.12f : 1.f), ((lastJump > 0.f) ? lastJump * 0.12f : 1.f));
						currentBlock = nullptr;
						return;
					}
					currentBlock->setOffset(offset);
					if(!currentBlock->hasParent()) currentBlock = nullptr;
				}
				else if(lastBlockTimer > 0) lastBlockTimer -= mFrameTime;
			}

			inline void move(int mDirection, float mFrameTime)
			{
				body.setVelocityX(walkSpeed * mDirection);

				if(mDirection != 0)
				{
					if(stepTime > 0.f) stepTime -= mFrameTime;
					else if(!cPhysics.isInAir())
					{
						game.getAssets().playSound("step.wav");
						stepTime = 26.f;
					}
				}
			}
			inline void jump()
			{
				if(cPhysics.isInAir() || !jumpReady) return;
				body.setVelocityY(-jumpSpeed);
				lastJump = 20.f;
				game.getAssets().playSound("jump.wav");
			}

			inline Action getAction()		{ return action; }
			inline bool isJumpReady()		{ return jumpReady; }
			inline bool isFacingLeft()		{ return facingLeft; }
			inline bool hasBlock()			{ return currentBlock != nullptr; }
	};
}

#endif
