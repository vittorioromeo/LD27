// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVLD_FACTORY
#define SSVLD_FACTORY

#include "LDDependencies.hpp"

namespace ld
{
    class LDAssets;
    class LDGame;
    class LDCRender;

    class LDFactory
    {
    private:
        std::unordered_map<int, sf::Color> colorMap;

        LDAssets& assets;
        LDGame& game;
        sses::Manager& manager;
        World& world;

        sf::Sprite getSpriteFromTile(const std::string& mTextureId,
            const sf::IntRect& mTextureRect) const;
        void emplaceSpriteFromTile(LDCRender& mCRender,
            const std::string& mTextureId,
            const sf::IntRect& mTextureRect) const;

        sses::Entity& createBlockBase(
            const ssvs::Vec2i& mPos, const ssvs::Vec2i& mSize, int mVal = -1);

    public:
        LDFactory(LDAssets& mAssets, LDGame& mGame, sses::Manager& mManager,
            World& mWorld)
            : assets(mAssets), game(mGame), manager(mManager), world(mWorld)
        {
        }

        sses::Entity& createWall(const ssvs::Vec2i& mPos);
        sses::Entity& createBlock(const ssvs::Vec2i& mPos, int mVal = -1);
        sses::Entity& createBlockBig(const ssvs::Vec2i& mPos, int mVal = -1);
        sses::Entity& createBlockBall(const ssvs::Vec2i& mPos, int mVal = -1);
        sses::Entity& createBlockRubberH(
            const ssvs::Vec2i& mPos, int mVal = -1);
        sses::Entity& createBlockRubberV(
            const ssvs::Vec2i& mPos, int mVal = -1);
        sses::Entity& createPlayer(const ssvs::Vec2i& mPos);
        sses::Entity& createReceiver(const ssvs::Vec2i& mPos, int mVal = -1);
        sses::Entity& createTele(const ssvs::Vec2i& mPos);
        sses::Entity& createLift(
            const ssvs::Vec2i& mPos, const ssvs::Vec2f& mVel);
    };
}

#endif
