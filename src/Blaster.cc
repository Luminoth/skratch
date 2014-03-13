/**
\file Blaster.cc
\author Shane Lillie
\brief Blaster item source.

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
#include "Blaster.h"
#include "Video.h"
#include "Audio.h"
#include "Skratch.h"
#include "World.h"
#include "state.h"


/*
 *  Blaster::BlasterShot constants
 *
 */


const std::string Blaster::BlasterShot::ANIMATION_FILENAME(DATADIR "/items/blaster/shot.tga");
const float Blaster::BlasterShot::HORIZONTAL_VEL = 650.0f;


/*
 *  Blaster::BlasterShot methods
 *
 */


Blaster::BlasterShot::BlasterShot(const Skratch& skratch, const World& world) : Entity()
{
    ENTER_FUNCTION(Blaster::BlasterShot::BlasterShot);

    m_mass = 0.0f;
    m_position = skratch.position().snap();
    m_position.add_y(world.block_height());

    switch(skratch.state())
    {
    case Skratch::IdleRight:
    case Skratch::RunningRight:
        m_position.add_x(world.block_width());
        m_velocity = Vector<float>(HORIZONTAL_VEL, 0.0f, 0.0f);
        break;
    case Skratch::IdleLeft:
    case Skratch::RunningLeft:
        m_velocity = Vector<float>(-HORIZONTAL_VEL, 0.0f, 0.0f);
        break;
    }
}


void Blaster::BlasterShot::think(const World& world)
{
    ENTER_FUNCTION(Blaster::BlasterShot::think);

    Vector<float> pos = position().snap();

    if(pos.x() < world.position().x() || pos.x() > (world.position().x() + Video::window_width()))
        set_removable();
}


void Blaster::BlasterShot::load_sprites(const VideoState& video_state)
{
    ENTER_FUNCTION(Blaster::BlasterShot::load_sprites);

    m_current_sprite_index = Entity::load_sprite(ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    Video::set_color_key(m_current_sprite_index, 0, 255, 0);
}


void Blaster::BlasterShot::on_animate(float dt, const std::bitset<World::CollisionSize>& collision_types, const World& world)
{
    ENTER_FUNCTION(Blaster::BlasterShot::on_animate);

    if(collision_types.count() > 0 || m_position.x() < 0.0f || m_position.y() < 0.0f)
        set_removable();
}


/*
 *  Blaster constants
 *
 */


const std::string Blaster::ANIMATION_FILENAME(DATADIR "/items/blaster/blaster.tga");
const std::string Blaster::SHOOT_SOUND_FILENAME(DATADIR "/items/blaster/shot.wav");
const float Blaster::COOL_TIME = 0.5f;


/*
 *  Blaster methods
 *
 */


Blaster::Blaster(bool add) : Entity(add), m_cool_time(0.0f), m_shoot_sound_index(-1)
{
    ENTER_FUNCTION(Blaster::Blaster);
}


void Blaster::shoot(const VideoState& video_state, const Skratch& skratch, const World& world)
{
    ENTER_FUNCTION(Blaster::shoot);

    if(m_cool_time > 0.0f) return;

    Audio::play_sound(m_shoot_sound_index);

    new BlasterShot(skratch, world);
    Entity::back()->load_media(video_state);

    m_cool_time = COOL_TIME;
}


void Blaster::lower_cool_time(float dt)
{
    ENTER_FUNCTION(Blaster::lower_cool_time);

    if(m_cool_time > 0.0f)
        m_cool_time -= dt;
}


void Blaster::think(const World& world)
{
    ENTER_FUNCTION(Blaster::think);
}


void Blaster::load_sprites(const VideoState& video_state)
{
    ENTER_FUNCTION(Blaster::load_sprites);

    m_current_sprite_index = Entity::load_sprite(ANIMATION_FILENAME, DefaultWidth, DefaultHeight, video_state);
    Video::set_color_key(m_current_sprite_index, 0, 255, 0);
}


void Blaster::load_sounds()
{
    ENTER_FUNCTION(Blaster::load_sounds);

    m_shoot_sound_index = Audio::load_sound(SHOOT_SOUND_FILENAME);
    Audio::set_sound_volume(m_shoot_sound_index, 64);
}
