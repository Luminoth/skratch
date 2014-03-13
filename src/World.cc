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
#include "World.h"
#include "Video.h"
#include "Skratch.h"
#include "Blaster.h"
#include "BlueCollarSuit.h"
#include "main.h"
#include "state.h"


// if this is defined, the world is scrolled parallaxically
#define X_PARALLAX
#define Y_PARALLAX


/*
 *  functions
 *
 */


inline int screen_to_grid_x(int x, int block_width)
{
    return x / block_width;
}


inline int screen_to_grid_y(int y, int block_height)
{
    return y / block_height;
}


inline int calc_grid_location(int column, int row, int grid_width)
{
    return (row * grid_width) + column;
}


inline int calc_grid_row(int grid_location, int grid_width)
{
    return grid_location / grid_width;
}


inline int calc_grid_column(int grid_location, int grid_width)
{
    return grid_location % grid_width;
}


/*
 *  World class constants
 *
 */


const float World::GRAVITY = 1550.0f;
const float World::FRICTION = 100.0f;


/*
 *  World methods
 *
 */


World::World()
    : m_width(0), m_height(0), m_block_width(0), m_block_height(0), m_blocks_wide(0), m_blocks_high(0), m_background_index(-1)
{
    ENTER_FUNCTION(World::World);
}


bool World::load(const std::string& name, const VideoState& video_state, Skratch* const skratch)
{
    ENTER_FUNCTION(World::load);

    // this makes things faster when rendering and such
    m_block_width = static_cast<int>(DefaultBlockWidth * video_state.width_scale);
    m_block_height = static_cast<int>(DefaultBlockHeight * video_state.height_scale);
    m_blocks_wide = Video::window_width() / m_block_width;
    m_blocks_high = Video::window_height() / m_block_height;

    const std::string path(DATADIR "/levels/" + name + "/");
    if(!load_collision_map(path)) return false;
    if(!load_texture_map(path)) return false;
    if(!load_entity_map(path, video_state)) return false;
    load_background(path);

/* FIXME: is this really what we want? maybe in the entity map instead? */
    skratch->set_position(Vector<float>(0.0f, static_cast<float>(m_position.y()), skratch->position().z()));

    return true;
}


void render_background(int background_index, int world_pixel_height, const Vector<int>& world_position)
{
    ENTER_FUNCTION(render_background);

    const int background_width = Video::at(background_index)->w;
    const int background_height = Video::at(background_index)->h;

#if defined X_PARALLAX
    const int x_scroll = (world_position.x() >> 1) % background_width;
#else
    const int x_scroll = world_position.x() % background_width;
#endif

#if defined Y_PARALLAX
    //const int y_scroll = (world_position.y() >> 1) % background_height;
    const int y_scroll = ((world_position.y() + Video::window_height()) >> 1) % background_height;
#else
    const int y_scroll = world_position.y() % background_height;
#endif

    SDL_Rect pos;
    pos.x = 0; pos.y = 0;

    SDL_Rect src;
    src.x = x_scroll; src.y = y_scroll;
    src.w = background_width - src.x;
    src.h = Video::window_height();

    Video::render_surface(background_index, &src, &pos);

    if(src.x != 0) {
        pos.x = src.w; src.y = 0;

        src.x = 0; src.y = y_scroll;
        src.w = background_width - pos.x;
        src.h = Video::window_height();

        Video::render_surface(background_index, &src, &pos);
    }
}


void World::render()
{
    ENTER_FUNCTION(World::render);

    const int start_location = calc_grid_location(screen_to_grid_x(m_position.x(), m_block_width), screen_to_grid_y(m_position.y(), m_block_height), m_width);
    const int start_x = calc_grid_column(start_location, m_width);
    const int start_y = calc_grid_row(start_location, m_width);

    if(m_background_index >= 0) render_background(m_background_index, m_height * m_block_height, m_position);

    SDL_Rect pos;
    pos.x = 0; pos.y = 0;

    SDL_Rect src;
    src.x = m_position.x() % m_block_width;
    src.y = m_position.y() % m_block_height;
    src.w = m_block_width - src.x;
    src.h = m_block_height - src.y;

    for(int y=0; y<=m_blocks_high; ++y) {
        for(int x=0; x<=m_blocks_wide; ++x) {
            const int location = calc_grid_location(x + start_x, y + start_y, m_width);
            if(m_blocks[location].surface_index >= 0)
                Video::render_surface(m_blocks[location].surface_index, &src, &pos);

            pos.x += src.w;
            src.x = 0; src.w = m_block_width;
        }
        pos.x = 0; pos.y += src.h;

        src.y = 0;
        src.x = m_position.x() % m_block_width;
        src.w = m_block_width - src.x;
        src.h = m_block_height;
    }
}


void World::scroll(const Skratch& skratch)
{
    ENTER_FUNCTION(World::scroll);

    const int half_window_width = Video::window_width() >> 1;
    const int half_window_height = Video::window_height() >> 1;
    const int window_x = static_cast<int>(skratch.position().x()) - m_position.x();
    const int window_y = static_cast<int>(skratch.position().y()) - m_position.y();

    const int pixel_width = m_width * m_block_width;
    const int pixel_height = m_height * m_block_height;

    if(window_x > half_window_width) m_position.set_x(m_position.x() + (window_x - half_window_width));
    else if(window_x < half_window_width) m_position.set_x(m_position.x() - (half_window_width - window_x));

    if(window_y > half_window_height) m_position.set_y(m_position.y() + (window_y - half_window_height));
    else if(window_y < half_window_height) m_position.set_y(m_position.y() - (half_window_height - window_y));

    if(m_position.x() < 0) m_position.clear_x();
    else if((m_position.x() + Video::window_width()) > pixel_width) m_position.set_x(pixel_width - Video::window_width());

    if(m_position.y() < 0) m_position.clear_y();
    else if((m_position.y() + Video::window_height()) > pixel_height) m_position.set_y(pixel_height - Video::window_height());
}


std::bitset<World::CollisionSize> World::collision(const Vector<float>& old_position, Vector<float>* const new_position, int entity_width, int entity_height) const
{
/*
A HUGE thanks goes out to MSW at the Gamedev (http://www.gamedev.net) forums
for helping me understand this approach to world collision detection.
*/

/*
FIXME: follow the path rather than just going straight to where we're at
*/

    ENTER_FUNCTION(World::collision);

    std::bitset<CollisionSize> ret;

    if(!new_position || entity_width < 0 || entity_height < 0) return ret;

    /*const int half_width = entity_width >> 1;
    const int half_height = entity_height >> 1;

    Vector<int> op = static_cast<Vector<int> >(old_position);*/
    Vector<int> np = static_cast<Vector<int> >(*new_position);

    int top_box = np.y() / m_block_height;
    int bottom_box = (np.y() + entity_height - 1) / m_block_height;
    int left_box = np.x() / m_block_width;
    int right_box = (np.x() + entity_width - 1) / m_block_width;

    int bottom_tile_top = bottom_box * m_block_height;
    int top_tile_bottom = (top_box * m_block_height) + m_block_height;

    for(int x=left_box; x<=right_box; ++x) {
        top_box = np.y() / m_block_height;
        bottom_box = (np.y() + entity_height - 1) / m_block_height;

        bottom_tile_top = bottom_box * m_block_height;
        top_tile_bottom = (top_box * m_block_height) + m_block_height;

        if(m_blocks[calc_grid_location(x, bottom_box, m_width)].type == MapBlock::Collidable) {
            np.set_y(bottom_tile_top - entity_height);
            ret[BottomCollision] = true;
        } else if(m_blocks[calc_grid_location(x, top_box, m_width)].type == MapBlock::Collidable) {
            np.set_y(top_tile_bottom);
            ret[TopCollision] = true;
        }
    }

    top_box = np.y() / m_block_height;
    bottom_box = (np.y() + entity_height - 1) / m_block_height;

    int right_tile_left = (right_box * m_block_width);
    int left_tile_right = (left_box * m_block_width) + m_block_width;

    for(int y=top_box; y<=bottom_box; ++y) {
        left_box = np.x() / m_block_width;
        right_box = (np.x() + entity_width - 1) / m_block_width;

        right_tile_left = (right_box * m_block_width);
        left_tile_right = (left_box * m_block_width) + m_block_width;

        if(m_blocks[calc_grid_location(left_box, y, m_width)].type == MapBlock::Collidable) {
            np.set_x(left_tile_right);
            ret[LeftCollision] = true;
        } else if(m_blocks[calc_grid_location(right_box, y, m_width)].type == MapBlock::Collidable) {
            np.set_x(right_tile_left - entity_width);
            ret[RightCollision] = true;
        }
    }

    if((np.x() + entity_width) > (m_width * m_block_width)) ret[EndWorld] = true;
    if((np.y() + entity_height) > (m_height * m_block_height)) ret[FellOffWorld] = true;

    *new_position = Vector<float>(np);
    return ret;
}


bool World::load_collision_map(std::string path)
{
    ENTER_FUNCTION(World::load_collision_map);

    path += "collision.map";
    std::ifstream infile(path.c_str());
    if(!infile) {
        std::cerr << "Couldn't open collision map - " << path << std::endl;
        return false;
    }

    char width, height;
    infile.get(width);
    infile.get(height);

    if(width < m_blocks_wide) {
        std::cerr << "World is not wide enough" << std::endl;
        infile.clear(); infile.close();
        return false;
    }
    if(height < m_blocks_high) {
        std::cerr << "World is not high enough" << std::endl;
        infile.clear(); infile.close();
        return false;
    }

    m_width = width;
    m_height = height;

    m_position = Vector<int>(0, (m_height * m_block_height) - Video::window_height(), 0);

    char c=0;
    infile.get(c);
    if(c != '\n') {
        std::cerr << "Invalid collision map" << std::endl;
        infile.clear(); infile.close();
        return false;
    }

    m_blocks.reserve(m_width * m_height);

    for(int y=0; y<m_height; ++y) {
        for(int x=0; x<m_width; ++x) {
            if(infile.eof()) {
                std::cerr << "Invalid collision map dimensions" << std::endl;
                infile.clear(); infile.close();
                return false;
            }

            infile.get(c);
            if(c - '0') m_blocks.push_back(MapBlock(MapBlock::Collidable, -1));
            else m_blocks.push_back(MapBlock(MapBlock::NotCollidable, -1));
        }

        infile.get(c);
        if(c != '\n') {
            std::cerr << "Invalid collision map" << std::endl;
            infile.clear(); infile.close();
            return false;
        }
    }
    infile.clear(); infile.close();
    return true;
}


bool World::load_texture_map(std::string path)
{
    ENTER_FUNCTION(World::load_texture_map);

    path += "texture.map";
    std::ifstream infile(path.c_str());
    if(!infile) {
        std::cerr << "Couldn't open texture map - " << path << std::endl;
        return false;
    }

    char c=0;
    for(int y=0; y<m_height; ++y) {
        for(int x=0; x<m_width; ++x) {
            if(infile.eof()) {
                std::cerr << "Invalid texture map dimensions" << std::endl;
                infile.clear(); infile.close();
                return false;
            }

            const int location = calc_grid_location(x, y, m_width);

            infile.get(c);
            if(c - '0') {
                char filename[256];
                snprintf(filename, 256,  DATADIR "/levels/blocks/block%02d.tga", c - '0');
                m_blocks[location].surface_index = Video::scale_surface(Video::load_image(filename), m_block_width, m_block_height);
            }
        }

        infile.get(c);
        if(c != '\n') {
            std::cerr << "Invalid texture map" << std::endl;
            infile.clear(); infile.close();
            return false;
        }
    }
    infile.clear(); infile.close();
    return true;
}


bool World::load_entity_map(std::string path, const VideoState& video_state)
{
    ENTER_FUNCTION(World::load_entity_map);

    path += "entity.map";
    std::ifstream infile(path.c_str());
    if(!infile) {
        std::cerr << "Couldn't open entity map - " << path << std::endl;
        return false;
    }

    char c=0;
    for(int y=0; y<m_height; ++y) {
        for(int x=0; x<m_width; ++x) {
            if(infile.eof()) {
                std::cerr << "Invalid entity map dimensions" << std::endl;
                infile.clear(); infile.close();
                return false;
            }

            infile.get(c);
            switch(c)
            {
            case '0': break;
            case 'B':
                new Blaster();
                Entity::back()->load_media(video_state);
                Entity::back()->set_position(Vector<float>(static_cast<float>(x * m_block_width), static_cast<float>(y * m_block_height), 0.0f));
                break;
            case 'S':
                new BlueCollarSuit();
                Entity::back()->load_media(video_state);
                Entity::back()->set_position(Vector<float>(static_cast<float>(x * m_block_width), static_cast<float>(y * m_block_height), 0.0f));
                break;
            default:
                std::cerr << "WARNING: Unknown entity, " << c << ", found in entity map" << std::endl;
            }
        }

        infile.get(c);
        if(c != '\n') {
            std::cerr << "Invalid texture map" << std::endl;
            infile.clear(); infile.close();
            return false;
        }
    }
    infile.clear(); infile.close();
    return true;
}


void World::load_background(std::string path)
{
    ENTER_FUNCTION(World::load_background);

    path += "background.tga";

    // file must exist
    struct stat buf;
    if(stat(path.c_str(), &buf)) return;

    m_background_index = Video::scale_surface(Video::load_image(path), Video::window_width(), m_height * m_block_height);
}


/*
 *  World friend functions
 *
 */


std::ostream& operator<<(std::ostream& lhs, const World& rhs)
{
    for(int y=0; y<rhs.m_height; ++y) {
        for(int x=0; x<rhs.m_width; ++x) {
            const int location = calc_grid_location(x, y, rhs.m_width);
            if(rhs.m_blocks[location].type == World::MapBlock::Collidable) lhs << "C";
            else lhs << "N";

            if(rhs.m_blocks[location].surface_index >= 0) lhs << rhs.m_blocks[location].surface_index;
            else lhs << " ";
        }
        lhs << std::endl;
    }
    return lhs;
}
