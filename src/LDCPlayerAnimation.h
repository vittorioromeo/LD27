// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_COMPONENTS_PLAYERANIMATION
#define SSVLD_COMPONENTS_PLAYERANIMATION

#include "LDDependencies.h"
#include "LDUtils.h"

namespace ld
{
	class LDCPlayerAnimation : public sses::Component
	{
		private:
			LDCRender& cRender;
			LDCPlayer& cPlayer;

			ssvs::Tileset& tileset;

			ssvs::Animation animTorsoStand, animTorsoJump, animTorsoFall, animTorsoWalk, animTorsoHold;
			ssvs::Animation animLegsStand, animLegsJump, animLegsFall, animLegsWalk;
			ssvs::Animation* currentTorsoAnim{nullptr};
			ssvs::Animation* currentLegsAnim{nullptr};

		public:
			LDCPlayerAnimation(ssvs::Tileset& mTileset, LDCRender& mCRender, LDCPlayer& mCPlayer) :
				 cRender(mCRender), cPlayer(mCPlayer), tileset(mTileset)
			{
				ssvuj::Obj animsTorso{ssvuj::readFromFile("Data/Animations/animCharTorso.json")};
				ssvuj::Obj animsLegs{ssvuj::readFromFile("Data/Animations/animCharLegs.json")};

				animTorsoStand = ssvs::getAnimationFromJson(tileset, animsTorso["stand"]);
				animTorsoJump = ssvs::getAnimationFromJson(tileset, animsTorso["jump"]);
				animTorsoFall = ssvs::getAnimationFromJson(tileset, animsTorso["fall"]);
				animTorsoWalk = ssvs::getAnimationFromJson(tileset, animsTorso["walk"]);
				animTorsoWalk.setType(ssvs::Animation::Type::PingPong); animTorsoWalk.setSpeed(0.75f);
				animTorsoHold = ssvs::getAnimationFromJson(tileset, animsTorso["hold"]);

				animLegsStand = ssvs::getAnimationFromJson(tileset, animsLegs["stand"]);
				animLegsJump = ssvs::getAnimationFromJson(tileset, animsLegs["jump"]);
				animLegsFall = ssvs::getAnimationFromJson(tileset, animsLegs["fall"]);
				animLegsWalk = ssvs::getAnimationFromJson(tileset, animsLegs["walk"]);
			}

			void update(float mFrameTime) override
			{
				using Action = LDCPlayer::Action;

				cRender.setFlippedX(cPlayer.isFacingLeft());
				cRender.setGlobalOffset({0, -6});

				switch(cPlayer.getAction())
				{
					case Action::Walking:
						currentTorsoAnim = &animTorsoWalk;
						currentLegsAnim = &animLegsWalk;
					break;
					case Action::Standing:
						currentTorsoAnim = &animTorsoStand;
						currentLegsAnim = &animLegsStand;
					break;
					case Action::Jumping:
						currentTorsoAnim = &animTorsoJump;
						currentLegsAnim = &animLegsJump;
					break;
					case Action::Falling:
						currentTorsoAnim = &animTorsoFall;
						currentLegsAnim = &animLegsFall;
					break;
				}

				if(cPlayer.hasBlock()) currentTorsoAnim = &animTorsoHold;

				if(currentTorsoAnim != nullptr)
				{
					currentTorsoAnim->update(mFrameTime);
					cRender[1].setTextureRect(tileset[currentTorsoAnim->getTileIndex()]);
				}

				if(currentLegsAnim != nullptr)
				{
					currentLegsAnim->update(mFrameTime);
					cRender[0].setTextureRect(tileset[currentLegsAnim->getTileIndex()]);
				}
			}
	};
}

#endif

