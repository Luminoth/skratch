/**
\file Entity.h
\author Shane Lillie
\brief Base entity class header.

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


#if !defined ENTITY_H
#define ENTITY_H


#include "shared.h"
#include "World.h"


class Audio;
struct VideoState;


class Entity
{
public:
    /**
    \brief Adds an entity.
    @param entity The entity to add.
    @note This doesn't copy the entity, it just adds the pointer.
    @note The entity must have been created with new.
    */
    static void push_back(Entity* const entity);

    /**
    @return The last entity.
    @retval NULL There are no entities.
    */
    static Entity* const back();

    /**
    \brief Causes all entities to think.
    @param world The game world.
    */
    static void all_think(const World& world);

    // animates all of the entities in the list
    /**
    \brief Animates all of the entities.
    @param dt The change in time in seconds.
    @param world The game world.
    */
    static void all_animate(float dt, const World& world);

    /**
    \brief Removes any entities that need removing.
    */
    static void cleanup();

    /**
    \brief Sorts the entities.
    */
    static void sort();

    /**
    \brief Renders all entities.
    @param world The game world.
    */
    static void render_entities(const World& world);

    /**
    \brief Frees all the entities.
    */
    static void free_entities();

    /**
    \brief Prints the entity info.
    @param out The output stream to print to.
    */
    static void print_entities(std::ostream& out);

private:
    static bool collision(const Entity& a, const Entity& b);

private:
    static std::list<Entity*> entities;

public:
    /**
    \brief Constructs a new entity
    @param add If this is true, the entity is added to the entity list.
    */
    explicit Entity(bool add=true);

    /**
    \brief Destroys the entity object.
    */
    virtual ~Entity() throw()
    {
    }

public:
    // loads all of the entities media (sounds, sprites, etc.)
    /**
    \brief Loads the entities media (sounds, sprites, etc.).
    @param video_state The video state.
    */
    void load_media(const VideoState& video_state);

    /**
    \brief Animates the entity and tests for world and entity collisions.
    @param dt The change in time in seconds.
    @param world The game world.
    @return A bitset containing the types of world collisions that occurred.
    @note This does world physics calculations, so they don't need to be done in sub-classes.
    */
    std::bitset<World::CollisionSize> animate(float dt, const World& world);

    /**
    \brief Renders the entity.
    @param world The game world.
    */
    void render(const World& world) const;

    /**
    @return The width of the entity.
    */
    int width() const;

    /**
    @return The height of the entity.
    */
    int height() const;

public:
    /**
    \brief Lets the entity think (should be, but doesn't have to be overloaded).
    @param world The game world.
    */
    virtual void think(const World& world) { }

    /**
    \brief Loads the entity's sprites.
    @param video_state The video state.
    */
    virtual void load_sprites(const VideoState& video_state) { }

    /**
    \brief Loads the entity's sounds.
    */
    virtual void load_sounds() { }

public:
    /**
    \brief Moves the entity to a position without collision detection.
    @param position The new position.
    */
    void set_position(const Vector<float>& position)
    {
        m_position = position;
    }

    /**
    \brief Sets the entity's velocity.
    @param velocity The new velocity.
    */
    void set_velocity(const Vector<float>& velocity)
    {
        m_velocity = velocity;
    }

    /**
    \brief Sets the entity's acceleration.
    @param acceleration The new acceleration.
    */
    void set_acceleration(const Vector<float>& acceleration)
    {
        m_acceleration = acceleration;
    }

    /**
    \brief Accelerates the entity.
    @param acceleration The acceleration amount.
    */
    void accelerate(const Vector<float>& acceleration)
    {
        m_acceleration += acceleration;
    }

    /**
    @return The entity's position.
    */
    const Vector<float>& position() const
    {
        return m_position;
    }

    /**
    @return The entity's velocity.
    */
    const Vector<float>& velocity() const
    {
        return m_velocity;
    }

    /**
    \brief Marks the entity to be removed.
    */
    void set_removable()
    {
        m_removable = true;
    }

    /**
    @retval true The entity will be removed on cleanup.
    @retval false The entity will not be removed on cleanup.
    */
    bool removable() const
    {
        return m_removable;
    }

public:
    /**
    \brief Compares entity positions.
    @param lhs The first entity to compare.
    @param rhs The second entity to compare.
    @note This compares z, then x, then y position.
    */
    friend bool operator<(const Entity& lhs, const Entity& rhs);

protected:
    /**
    \brief Loads a sprite for the entity.
    @param filename The file to load.
    @param width The width to make the sprite.
    @param height The height to make the sprite.
    @param video_state The video state.
    @return The index of the sprite.
    @note This will scale width and height by the video scale factor, so don't do it to the arguments.
    */
    int load_sprite(const std::string& filename, int width, int height, const VideoState& video_state);

protected:
    /**
    \brief Sets the state of the entity.
    */
    virtual void set_state(int state) { }

    /**
    \brief This is called when the entity is animated.
    @param dt The change in time in seconds.
    @param collision_types The types of collisions that occurred.
    @param world The game world.
    */
    virtual void on_animate(float dt, const std::bitset<World::CollisionSize>& collision_types, const World& world) { }

    /**
    \brief Called when an entity collision occurs.
    @param entity The entity that this entity collided with.
    */
    virtual void on_collision(Entity* const entity) { }

protected:
    Vector<float> m_position;
    Vector<float> m_velocity;
    Vector<float> m_acceleration;

    int m_current_sprite_index;

    float m_animation_seconds, m_mass;

private:
    bool m_removable;
};


#endif
