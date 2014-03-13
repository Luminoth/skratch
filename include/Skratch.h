/*
==========
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
==========
*/


#if !defined SKRATCH_H
#define SKRATCH_H


#include "shared.h"
#include "Entity.h"
#include "Blaster.h"
#include "World.h"


struct VideoState;


class Skratch : public Entity
{
public:
    enum SkratchState
    {
        IdleRight,
        IdleLeft,
        RunningRight,
        RunningLeft
    };

private:
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
        JumpSound,

        // this should match the number of sounds
        SoundCount = 1
    };

    static const std::string IDLE_ANIMATION_FILENAME;
    static const std::string RUN1_ANIMATION_FILENAME;
    static const std::string RUN2_ANIMATION_FILENAME;

    static const std::string JUMP_SOUND_FILENAME;

    static const float MASS;
    static const float MAX_HORIZONTAL_VEL;
    static const float HORIZONTAL_ACCEL;
    static const float JUMP_VEL;

public:
    Skratch();
    virtual ~Skratch() throw()
    {
    }

public:
    // skratch has specialized animate and think functions
    std::bitset<World::CollisionSize> animate(float dt, World* const world);
    virtual void think(const World& world) {}
    void think(const VideoState& video_state, bool* const keystate, const World& world);

public:
    virtual void load_sprites(const VideoState& video_state);
    virtual void load_sounds();

public:
    bool has_blaster() const { return m_blaster.get() != NULL; }
    SkratchState state() const { return m_state; }

private:
    virtual void on_animate(float dt, const std::bitset<World::CollisionSize>& collision_types, const World& world);
    virtual void on_collision(Entity* const entity);
    virtual void set_state(int state);

private:
    bool m_can_jump;

    int m_sprite_indexes[AnimationCount];
    int m_sound_indexes[SoundCount];

    SkratchState m_state;

    std::auto_ptr<Blaster> m_blaster;

private:
    Skratch(const Skratch& skratch) {}
    const Skratch& operator=(const Skratch& rhs) { return *this; }
};


#endif
