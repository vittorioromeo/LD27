// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_COMPONENTS_RENDER
#define SSVLD_COMPONENTS_RENDER

#include "LDDependencies.hpp"
#include "LDGame.hpp"
#include "LDUtils.hpp"

namespace ld
{
	class LDGame;

	class LDCRender : public sses::Component
	{
		private:
			LDGame& game;
			Body& body;
			std::vector<sf::Sprite> sprites;
			std::vector<ssvs::Vec2f> offsets;
			bool flippedX{false}, flippedY{false}, scaleWithBody{false};
			ssvs::Vec2f globalOffset;

		public:
			inline LDCRender(sses::Entity& mE, LDGame& mGame, Body& mBody) : sses::Component{mE}, game(mGame), body(mBody) { }

			inline void update(FT) override
			{
				const auto& position(toPixels(body.getPosition()));
				const auto& size(toPixels(body.getSize()));

				for(auto i(0u); i < sprites.size(); ++i)
				{
					auto& s(sprites[i]);

					const auto& rect(s.getTextureRect());
					s.setOrigin({rect.width / 2.f, rect.height / 2.f});
					s.setPosition(position + globalOffset + offsets[i]);
					s.setScale(flippedX ? -1 : 1, flippedY ? -1 : 1);

					if(scaleWithBody) s.setScale(size.x / rect.width, size.y / rect.height);
				}
			}
			inline void draw() override { for(const auto& s : sprites) game.render(s); }

			template<typename... TArgs> inline void emplaceSprite(TArgs&&... mArgs) { sprites.emplace_back(FWD(mArgs)...); offsets.emplace_back(); }

			inline void setRotation(float mDegrees)	noexcept					{ for(auto& s : sprites) s.setRotation(mDegrees); }
			inline void setFlippedX(bool mFlippedX)	noexcept					{ flippedX = mFlippedX; }
			inline void setFlippedY(bool mFlippedY)	noexcept					{ flippedY = mFlippedY; }
			inline void setScaleWithBody(bool mScale) noexcept					{ scaleWithBody = mScale; }
			inline void setGlobalOffset(const ssvs::Vec2f& mOffset) noexcept	{ globalOffset = mOffset; }

			inline bool isFlippedX() const noexcept							{ return flippedX; }
			inline bool isFlippedY() const noexcept							{ return flippedY; }
			inline const decltype(sprites)& getSprites() const noexcept		{ return sprites; }
			inline const decltype(offsets)& getOffsets() const noexcept		{ return offsets; }
			inline decltype(sprites)& getSprites() noexcept					{ return sprites; }
			inline decltype(offsets)& getOffsets() noexcept					{ return offsets; }
			inline sf::Sprite& operator[](unsigned int mIdx)				{ return sprites[mIdx]; }
			inline const sf::Sprite& operator[](unsigned int mIdx) const	{ return sprites[mIdx]; }
	};
}

#endif
