/**
\file BlueCollarSuit.h
\author Shane Lillie
\brief BlueCollarSuit entity header.

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

#if !defined BLUECOLLARSUIT_H
#define BLUECOLLARSUIT_H


#include "shared.h"
#include "Entity.h"
#include "World.h"


struct VideoState;


/**
\class BlueCollarSuit
\brief BlueCollarSuit entity class.
*/
class BlueCollarSuit : public Entity
{
private:
    enum BlueCollarSuitState
    {
        IdleRight,
        IdleLeft,
        RunningRight,
        RunningLeft
    };

    enum
    {
        DefaultWidth  = 32,
        DefaultHeight = 64
    };

    enum
    {
        IdleLeftAnimation,
        IdleRightAnimation,
        Run1LeftAnimation,
        Run2LeftAnimation,
        Run1RightAnimation,
        Run2RightAnimation,

        // this should match the number of animations
        AnimationCount = 6
    };

    enum
    {
        // this should match the number of sounds
        SoundCount = 0
    };

    static const std::string IDLE_ANIMATION_FILENAME;
    static const std::string RUN1_ANIMATION_FILENAME;
    static const std::string RUN2_ANIMATION_FILENAME;

    static const float MASS;
    static const float MAX_HORIZONTAL_VEL;
    static const float HORIZONTAL_ACCEL;

public:
    /**
    \brief Constructs a BlueCollarSuit object.
    */
    BlueCollarSuit();

public:
    virtual void think(const World& world);
    virtual void load_sprites(const VideoState& video_state);
    virtual void load_sounds();

private:
    virtual void on_animate(float dt, const std::bitset<World::CollisionSize>& collision_types, const World& world);
    virtual void on_collision(Entity* const entity);
    virtual void set_state(int state);

private:
    int m_sprite_indexes[AnimationCount];
//    int m_sound_indexes[SoundCount];

    BlueCollarSuitState m_state;

private:
    BlueCollarSuit(const BlueCollarSuit& skratch) {}
    const BlueCollarSuit& operator=(const BlueCollarSuit& rhs) { return *this; }
};


#endif
