/**
\file Blaster.h
\author Shane Lillie
\brief Blaster item header.

\verbatim
Copyright 2002 Energon Software

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
\endverbatim
*/

#if !defined BLASTER_H
#define BLASTER_H


#include "shared.h"
#include "Entity.h"


class Skratch;


/**
\class Blaster
\brief Blaster item.
*/
class Blaster : public Entity
{
public:
    /**
    \class BlasterShot
    \brief A shot from the blaster item.
    */
    class BlasterShot : public Entity
    {
    private:
        enum
        {
            DefaultWidth = 8,
            DefaultHeight = 6
        };

        static const std::string ANIMATION_FILENAME;
        static const float HORIZONTAL_VEL;

    public:
        /**
        \brief Constructs a new BlasterShot.
        @param skratch The Skratch entity that shot.
        @param world The game world.
        */
        BlasterShot(const Skratch& skratch, const World& world);

    public:
        virtual void think(const World& world);
        virtual void load_sprites(const VideoState& video_state);

    private:
        virtual void on_animate(float dt, const std::bitset<World::CollisionSize>& collision_types, const World& world);
    };

private:
    enum
    {
        DefaultWidth  = 16,
        DefaultHeight = 16
    };

    static const std::string ANIMATION_FILENAME;
    static const std::string SHOOT_SOUND_FILENAME;

    static const float COOL_TIME;

public:
    /**
    \brief Constructs a new Blaster item.
    @param add If this is true, the blaster is added to the entity list.
    */
    explicit Blaster(bool add=true);

public:
    /**
    \brief Shoots the blaster and starts the cool down timer.
    @param video_state The video state.
    @param skratch The Skratch entity that's shooting.
    @param world The game world.
    */
    void shoot(const VideoState& video_state, const Skratch& skratch, const World& world);

    /**
    \brief Lowers the cool down time.
    @param dt The change in time in seconds to cool down by.
    */
    void lower_cool_time(float dt);

public:
    virtual void think(const World& world);
    virtual void load_sprites(const VideoState& video_state);
    virtual void load_sounds();

private:
    float m_cool_time;

    int m_shoot_sound_index;
};


#endif
