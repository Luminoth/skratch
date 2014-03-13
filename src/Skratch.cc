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


#include "shared.h"
#include "Skratch.h"
#include "Video.h"
#include "Audio.h"
#include "World.h"
#include "Blaster.h"
#include "BlueCollarSuit.h"
#include "state.h"


/*
 *  Skratch constants
 *
 */


const std::string Skratch::IDLE_ANIMATION_FILENAME(DATADIR "/characters/skratch/idle.tga");
const std::string Skratch::RUN1_ANIMATION_FILENAME(DATADIR "/characters/skratch/run1.tga");
const std::string Skratch::RUN2_ANIMATION_FILENAME(DATADIR "/characters/skratch/run2.tga");

const std::string Skratch::JUMP_SOUND_FILENAME(DATADIR "/characters/skratch/jump.wav");

const float Skratch::MASS = 1;
const float Skratch::MAX_HORIZONTAL_VEL = 325.0f;
const float Skratch::HORIZONTAL_ACCEL = 525.0f;
const float Skratch::JUMP_VEL = 650.0f;



/*
 *  Skratch methods
 *
 */


Skratch::Skratch() : Entity(false), m_can_jump(false), m_state(IdleRight), m_blaster(NULL)
{
    ENTER_FUNCTION(Skratch::Skratch);

    m_mass = MASS;
    memset(m_sprite_indexes, -1, AnimationCount * sizeof(int));
    memset(m_sound_indexes, -1, SoundCount * sizeof(int));
}


std::bitset<World::CollisionSize> Skratch::animate(float dt, World* const world)
{
    ENTER_FUNCTION(Skratch::animate);

    std::bitset<World::CollisionSize> ret = Entity::animate(dt, *world);
    world->scroll(*this);
    return ret;
}


void Skratch::think(const VideoState& video_state, bool* const keystate, const World& world)
{
    ENTER_FUNCTION(Skratch::think);

    if(keystate[SDLK_RIGHT]) {
        m_acceleration./*add*/set_x(HORIZONTAL_ACCEL);
        set_state(RunningRight);
    } else if(keystate[SDLK_LEFT]) {
        m_acceleration./*add*/set_x(-HORIZONTAL_ACCEL);
        set_state(RunningLeft);
    } else {
        m_acceleration.clear_x();

        if(m_state == RunningRight) set_state(IdleRight);
        else if(m_state == RunningLeft) set_state(IdleLeft);
    }

    /*} else {
        // slow down gradually
        m_velocity.add_x(m_velocity.x() / -2.0f);
        m_velocity.snap_x();

        m_acceleration.clear_x();

        if(m_state == RunningRight) set_state(IdleRight);
        else if(m_state == RunningLeft) set_state(IdleLeft);
    }*/

    if(keystate[SDLK_SPACE] && m_can_jump) {
        keystate[SDLK_SPACE] = false;
        Audio::play_sound(m_sound_indexes[JumpSound]);
        m_velocity./*set*/add_y(-JUMP_VEL);
    } else m_acceleration.clear_y();

    if((keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) && m_blaster.get())
        m_blaster->shoot(video_state, *this, world);
}


void Skratch::load_sprites(const VideoState& video_state)
{
    ENTER_FUNCTION(Skratch::load_sprites);

    m_sprite_indexes[IdleRightAnimation] = Entity::load_sprite(IDLE_ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    m_sprite_indexes[IdleLeftAnimation]  = Video::flip_surface_horizontal(m_sprite_indexes[IdleRightAnimation], "skratch_idle_left");
    m_sprite_indexes[Run1RightAnimation] = Entity::load_sprite(RUN1_ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    m_sprite_indexes[Run2RightAnimation] = Entity::load_sprite(RUN2_ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    m_sprite_indexes[Run1LeftAnimation]  = Video::flip_surface_horizontal(m_sprite_indexes[Run1RightAnimation], "skratch_run1_left");
    m_sprite_indexes[Run2LeftAnimation]  = Video::flip_surface_horizontal(m_sprite_indexes[Run2RightAnimation], "skratch_run2_left");

    for(int i=0; i<AnimationCount; ++i)
        Video::set_color_key(m_sprite_indexes[i], 0, 255, 0);

    m_current_sprite_index = m_sprite_indexes[IdleRightAnimation];
}


void Skratch::load_sounds()
{
    ENTER_FUNCTION(Skratch::load_sounds);

    m_sound_indexes[JumpSound] = Audio::load_sound(JUMP_SOUND_FILENAME);
}


void Skratch::on_animate(float dt, const std::bitset<World::CollisionSize>& collision_types, const World& world)
{
    ENTER_FUNCTION(Skratch::on_animate);

    if(m_position.x() < 0.0f)
        m_position.clear_x();
    if(m_position.y() < 0.0f)
        m_position.clear_y();

    m_can_jump = collision_types[World::BottomCollision];

    // don't go too fast
    if(m_velocity.x() > MAX_HORIZONTAL_VEL)
        m_velocity.set_x(MAX_HORIZONTAL_VEL);
    else if(m_velocity.x() < -MAX_HORIZONTAL_VEL)
        m_velocity.set_x(-MAX_HORIZONTAL_VEL);

    if(has_blaster())
        m_blaster->lower_cool_time(dt);

    m_animation_seconds += dt;
    switch(m_state)
    {
    case RunningLeft:
        if(m_animation_seconds >= 0.0f && m_animation_seconds < 0.25f)
            m_current_sprite_index = m_sprite_indexes[Run1LeftAnimation];
        else if(m_animation_seconds >= 0.25f && m_animation_seconds < 0.5f)
            m_current_sprite_index = m_sprite_indexes[IdleLeftAnimation];
        else if(m_animation_seconds >= 0.5f && m_animation_seconds < 0.75f)
            m_current_sprite_index = m_sprite_indexes[Run2LeftAnimation];
        else if(m_animation_seconds >= 0.75f && m_animation_seconds < 1.0f)
            m_current_sprite_index = m_sprite_indexes[IdleLeftAnimation];
        else
            m_animation_seconds = 0.0f;
        break;
    case RunningRight:
        if(m_animation_seconds >= 0.0f && m_animation_seconds < 0.25f)
            m_current_sprite_index = m_sprite_indexes[Run1RightAnimation];
        else if(m_animation_seconds >= 0.25f && m_animation_seconds < 0.5f)
            m_current_sprite_index = m_sprite_indexes[IdleRightAnimation];
        else if(m_animation_seconds >= 0.5f && m_animation_seconds < 0.75f)
            m_current_sprite_index = m_sprite_indexes[Run2RightAnimation];
        else if(m_animation_seconds >= 0.75f && m_animation_seconds < 1.0f)
            m_current_sprite_index = m_sprite_indexes[IdleRightAnimation];
        else
            m_animation_seconds = 0.0f;
        break;
    default: break;
    }

//std::cout << "Skratch's position: " << m_position << "\tvelocity: " << m_velocity << "\tacceleration: " << m_acceleration << std::endl;
}


void Skratch::on_collision(Entity* const entity)
{
    ENTER_FUNCTION(Skratch::on_collision);

    if(typeid(*entity) == typeid(Blaster)) {
        entity->set_removable();

        std::auto_ptr<Blaster> b(new Blaster(false));
        m_blaster = b;
        m_blaster->load_sounds();
    } else if(typeid(*entity) == typeid(BlueCollarSuit))
        set_removable();
}


void Skratch::set_state(int state)
{
    ENTER_FUNCTION(Skratch::set_state);

    switch(state)
    {
    case IdleLeft:
        m_current_sprite_index = m_sprite_indexes[IdleLeftAnimation];
        break;
    case IdleRight:
        m_current_sprite_index = m_sprite_indexes[IdleRightAnimation];
        break;
    case RunningLeft:
        m_current_sprite_index = m_sprite_indexes[Run1LeftAnimation];
        break;
    case RunningRight:
        m_current_sprite_index = m_sprite_indexes[Run1RightAnimation];
        break;
    default: return;
    }

    if(state != m_state)
        m_animation_seconds = 0.0f;
    m_state = static_cast<SkratchState>(state);
}
