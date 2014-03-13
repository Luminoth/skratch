/**
\file BlueCollarSuit.cc
\author Shane Lillie
\brief BlueCollarSuit entity source.

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


#include "shared.h"
#include "BlueCollarSuit.h"
#include "Video.h"
#include "Skratch.h"


/*
 *  BlueCollarSuit constants
 *
 */


const std::string BlueCollarSuit::IDLE_ANIMATION_FILENAME(DATADIR "/characters/bluecollarsuit/idle.tga");
const std::string BlueCollarSuit::RUN1_ANIMATION_FILENAME(DATADIR "/characters/bluecollarsuit/run1.tga");
const std::string BlueCollarSuit::RUN2_ANIMATION_FILENAME(DATADIR "/characters/bluecollarsuit/run2.tga");

const float BlueCollarSuit::MASS = 1;
const float BlueCollarSuit::MAX_HORIZONTAL_VEL = 75.0f;
const float BlueCollarSuit::HORIZONTAL_ACCEL = 225.0f;


/*
 *  BlueCollarSuit methods
 *
 */


BlueCollarSuit::BlueCollarSuit() : Entity(), m_state(IdleRight)
{
    ENTER_FUNCTION(BlueCollarSuit::BlueCollarSuit);

    m_mass = MASS;
    memset(m_sprite_indexes, -1, AnimationCount * sizeof(int));
//    memset(m_sound_indexes, -1, SoundCount * sizeof(int));
}


void BlueCollarSuit::think(const World& world)
{
    ENTER_FUNCTION(BlueCollarSuit::think);
}


void BlueCollarSuit::load_sprites(const VideoState& video_state)
{
    ENTER_FUNCTION(BlueCollarSuit::load_sprites);

    m_sprite_indexes[IdleRightAnimation] = Entity::load_sprite(IDLE_ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    m_sprite_indexes[IdleLeftAnimation]  = Video::flip_surface_horizontal(m_sprite_indexes[IdleRightAnimation], "bluecollarsuit_idle_left");
    m_sprite_indexes[Run1RightAnimation] = Entity::load_sprite(RUN1_ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    m_sprite_indexes[Run2RightAnimation] = Entity::load_sprite(RUN2_ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    m_sprite_indexes[Run1LeftAnimation]  = Video::flip_surface_horizontal(m_sprite_indexes[Run1RightAnimation], "bluecollarsuit_run1_left");
    m_sprite_indexes[Run2LeftAnimation]  = Video::flip_surface_horizontal(m_sprite_indexes[Run2RightAnimation], "bluecollarsuit_run2_left");

    for(int i=0; i<AnimationCount; ++i)
        Video::set_color_key(m_sprite_indexes[i], 0, 255, 0);

    m_current_sprite_index = m_sprite_indexes[IdleRightAnimation];
}


void BlueCollarSuit::load_sounds()
{
    ENTER_FUNCTION(BlueCollarSuit::load_sounds);
}


void BlueCollarSuit::on_animate(float dt, const std::bitset<World::CollisionSize>& collision_types, const World& world)
{
    ENTER_FUNCTION(BlueCollarSuit::on_animate);

    if(m_position.x() < 0.0f)
        m_position.clear_x();
    if((m_position.x() + width()) > world.pixel_width())
        m_position.set_x(world.pixel_width() - width());
    if(m_position.y() < 0.0f)
        m_position.clear_y();
    if((m_position.y() + height()) > world.pixel_height())
        m_position.set_y(world.pixel_height() - height());

    // don't go too fast
    if(m_velocity.x() > MAX_HORIZONTAL_VEL)
        m_velocity.set_x(MAX_HORIZONTAL_VEL);
    else if(m_velocity.x() < -MAX_HORIZONTAL_VEL)
        m_velocity.set_x(-MAX_HORIZONTAL_VEL);

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
}


void BlueCollarSuit::on_collision(Entity* const entity)
{
    ENTER_FUNCTION(BlueCollarSuit::on_collision);

    if(typeid(*entity) == typeid(Blaster::BlasterShot)) {
        set_removable();
        entity->set_removable();
    }
}


void BlueCollarSuit::set_state(int state)
{
    ENTER_FUNCTION(BlueCollarSuit::set_state);

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
    m_state = static_cast<BlueCollarSuitState>(state);
}
