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

#if !defined WORLD_H
#define WORLD_H


#include "shared.h"


class Skratch;
struct VideoState;


/*
 *  World class
 *
 */


class World
{
private:
    struct MapBlock
    {
        enum Type
        {
            Collidable,
            NotCollidable
        };

        Type type;
        int surface_index;

        MapBlock(Type t, int i) : type(t), surface_index(i)
        {
        }
    };

public:
    static const float GRAVITY;
    static const float FRICTION;

    enum
    {
        BottomCollision,
        TopCollision,
        RightCollision,
        LeftCollision,
        FellOffWorld,
        EndWorld,

        // this should be the # of collision bits to use
        CollisionSize = 6
    };

private:
    enum
    {
        DefaultBlockWidth  = 32,
        DefaultBlockHeight = 32
    };

public:
    World();

public:
    // loads a world
    // worlds are in '$(DATADIR)/levels/name/'
    // positions skratch where he should be in the map
    bool load(const std::string& name, const VideoState& video_state, Skratch* const skratch);

    // renders the world to the window
    // renders all entities but Skratch
    void render();

    // scrolls the world in a direction
    void scroll(const Skratch& skratch);

    // tests for entity collisions in the world
    // sets new_position to the position the entity would stop at
    // returns the set of collisions that occured
    std::bitset<CollisionSize> collision(const Vector<float>& old_position, Vector<float>* const new_position, int entity_width, int entity_height) const;

public:
    const Vector<int>& position() const { return m_position; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    int block_width() const { return m_block_width; }
    int block_height() const { return m_block_height; }

    int pixel_width() const { return m_width * m_block_width; }
    int pixel_height() const { return m_height * m_block_height; }

private:
    bool load_collision_map(std::string path);
    bool load_texture_map(std::string path);
    bool load_entity_map(std::string path, const VideoState& video_state);
    void load_background(std::string path);

public:
    // outputs the world to an output stream
    friend std::ostream& operator<<(std::ostream& lhs, const World& rhs);

private:
    Vector<int> m_position;

    std::vector<MapBlock> m_blocks;

    int m_width, m_height;
    int m_block_width, m_block_height;
    int m_blocks_wide, m_blocks_high;

    int m_background_index;
};


#endif
