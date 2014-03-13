/**
\file Entity.cc
\author Shane Lillie
\brief Base entity class source

\verbatim
Copyright 2002-2003 Energon Software

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
#include "Entity.h"
#include "Video.h"
#include "World.h"
#include "state.h"


/*
 *  Entity class variables
 *
 */


std::list<Entity*> Entity::entities;


/*
 *  Entity class functions
 *
 */


void Entity::push_back(Entity* const entity)
{
    ENTER_FUNCTION(Entity::push_back);

    entities.push_back(entity);
}


Entity* const Entity::back()
{
    ENTER_FUNCTION(Entity::back);

    if(!entities.size())
        return NULL;
    return entities.back();
}


void Entity::all_think(const World& world)
{
    ENTER_FUNCTION(Entity::all_think);

    for(std::list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it) {
        if(*it && !(*it)->removable()) {
            (*it)->think(world);
        }
    }
}


void Entity::all_animate(float dt, const World& world)
{
    ENTER_FUNCTION(Entity::all_animate);

    for(std::list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it) {
        if(*it && !(*it)->removable()) {
            (*it)->animate(dt, world);
        }
    }
}


void Entity::cleanup()
{
    ENTER_FUNCTION(Entity::cleanup);

    for(std::list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it) {
        if(*it) {
            if((*it)->removable()) {
                delete (*it);
                *it = NULL;
            }
        }
    }
    entities.remove(NULL);
}


void Entity::sort()
{
    ENTER_FUNCTION(Entity::sort);

    entities.sort();
}


void Entity::render_entities(const World& world)
{
    ENTER_FUNCTION(Entity::render_entities);

    for(std::list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it) {
        if(*it) {
            (*it)->render(world);
        }
    }
}


void Entity::free_entities()
{
    ENTER_FUNCTION(Entity::free_entities);

    for(std::list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it) {
        if(*it) delete (*it);
        *it = NULL;
    }
    entities.clear();
}


void Entity::print_entities(std::ostream& out)
{
    ENTER_FUNCTION(Entity::print_entities);

    out << "I have " << entities.size() << " entities" << std::endl;

    int i=0;
    for(std::list<Entity*>::const_iterator it = entities.begin(); it != entities.end(); ++it)
        out << i++ << ": " << typeid(*(*it)).name() << std::endl;
}


/**
\brief Tests for a collision between two entities.
@param a The first entity to test with.
@param b The second entity to test with.
@note This does position based detection, and is therefore not good.
@todo Do a time-based collision detection.
*/
bool Entity::collision(const Entity& a, const Entity& b)
{
    ENTER_FUNCTION(Entity::collision);

/* FIXME: should we snap the positions? */

    if((a.position().x() <= b.position().x() && (a.position().x() + a.width()) >= b.position().x()) &&
        (a.position().y() <= b.position().y() && (a.position().y() + a.height()) >= b.position().y()) ||
        (a.position().x() >= b.position().x() && a.position().x() <= (b.position().x() + b.width())) &&
        (a.position().y() >= b.position().y() && a.position().y() <= (b.position().y() + b.width())))
        return true;

/* FIXME: any more comparisons needed? */

    return false;
}


/*
 *  Entity methods
 *
 */


Entity::Entity(bool add) : m_current_sprite_index(-1), m_animation_seconds(0.0f), m_mass(0.0f), m_removable(false)
{
    ENTER_FUNCTION(Entity::Entity);

    if(add)
        push_back(this);
}


void Entity::load_media(const VideoState& video_state)
{
    ENTER_FUNCTION(Entity::load_media);

    load_sprites(video_state);
    load_sounds();
}


std::bitset<World::CollisionSize> Entity::animate(float dt, const World& world)
{
    ENTER_FUNCTION(Entity::animate);

    std::bitset<World::CollisionSize> wc;
    if(width() < 0 || height() < 0) {
        on_animate(dt, wc, world);
        return wc;
    }

    // gravity
    m_acceleration.add_y(World::GRAVITY * m_mass);

    // s2 = s1 + (v1 * t) + ((a * t^2) / 2)
    Vector<float> new_position = m_position + (m_velocity * dt) + ((m_acceleration * dt * dt) / 2);
    wc = world.collision(m_position, &new_position, width(), height());

    // respond to the world collisions
    if(wc[World::LeftCollision] || wc[World::RightCollision]) {
        m_velocity.clear_x();
        m_acceleration.clear_x();
    }
    if(wc[World::BottomCollision] || wc[World::TopCollision]) {
        m_velocity.clear_y();
        m_acceleration.clear_y();

        // friction
        m_acceleration.add_x(m_velocity.x() != 0.0f ? m_velocity.x() > 1.0f ? -World::FRICTION * m_mass : World::FRICTION * m_mass : 0.0f);
    } else {
        // v2 = v1 + (a * t)
        m_velocity = m_velocity + (m_acceleration * dt);
    }

    // check for entity-entity collisions
    for(std::list<Entity*>::const_iterator it = entities.begin(); it != entities.end(); ++it) {
        if(collision(*this, *(*it))) {
            on_collision(*it);
        }
    }
    m_position = new_position;

    on_animate(dt, wc, world);
    return wc;
}


void Entity::render(const World& world) const
{
    ENTER_FUNCTION(Entity::render);

    if(m_current_sprite_index < 0) return;

    Vector<int> pos = static_cast<Vector<int> >(m_position);

    // only render if we're on-screen
    if((pos.y() + height()) < world.position().y() ||
        pos.y() > (world.position().y() + Video::window_height()) ||
        (pos.x() + width()) < world.position().x() ||
        pos.x() > (world.position().x() + Video::window_width()))
        return;

    SDL_Rect rect;
    rect.x = pos.x() - world.position().x();
    rect.y = pos.y() - world.position().y();

    Video::render_surface(m_current_sprite_index, NULL, &rect);
}


int Entity::width() const
{
    ENTER_FUNCTION(Entity::width);

    return (m_current_sprite_index >= 0) ? Video::at(m_current_sprite_index)->w : -1;
}


int Entity::height() const
{
    ENTER_FUNCTION(Entity::height);

    return (m_current_sprite_index >= 0) ? Video::at(m_current_sprite_index)->h : -1;
}


int Entity::load_sprite(const std::string& filename, int width, int height, const VideoState& video_state)
{
    ENTER_FUNCTION(Entity::load_sprite);

    return Video::scale_surface(Video::load_image(filename), static_cast<int>(width * video_state.width_scale), static_cast<int>(height * video_state.height_scale));
}


/*
 *  Entity friend functions
 *
 */


bool operator<(const Entity& lhs, const Entity& rhs)
{
    if(lhs.m_position.z() < rhs.m_position.z()) return true;
    else if(rhs.m_position.z() < lhs.m_position.z()) return false;

    if(lhs.m_position.x() < rhs.m_position.x()) return true;
    else if(rhs.m_position.x() < lhs.m_position.x()) return false;

    if(lhs.m_position.y() < rhs.m_position.y()) return true;
    else if(rhs.m_position.y() < lhs.m_position.y()) return false;

    return false;
}
