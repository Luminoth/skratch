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
#include "Font.h"
#include "Video.h"
#include "state.h"


/*
 *  Font methods
 *
 */


Font::Font() : m_surface_index(-1), m_char_width(0), m_char_height(0)
{
    ENTER_FUNCTION(Font::Font);
}


bool Font::load(const std::string& filename, const VideoState& video_state)
{
    ENTER_FUNCTION(Font::load);

    m_surface_index = Video::scale_surface(Video::load_image(filename), static_cast<int>(DefaultWidth * video_state.width_scale), static_cast<int>(DefaultHeight * video_state.height_scale));
    if(m_surface_index < 0) return false;

    Video::set_color_key(m_surface_index, 0, 0, 0);

    // calculate the character width/height
    m_char_width = Video::at(m_surface_index)->w >> 4;
    m_char_height = Video::at(m_surface_index)->h >> 4;
    return true;
}


void Font::render_text(const std::string& text, SDL_Surface* const destination, int x, int y) const
{
    ENTER_FUNCTION(Font::render_text);

    if(!destination || m_surface_index < 0) return;

    SDL_Rect pos;
    pos.x = x; pos.y = y;

    for(unsigned int i=0; i<text.length(); ++i) {
        if((pos.x + m_char_width) > destination->w) break;
        if((pos.y + m_char_height) > destination->h) break;

        const char cur_char = text[i];

        // handle some special characters
        switch(cur_char)
        {
        case '\n':
            pos.x = x;
            pos.y += m_char_height;
            continue;
        case '\r':
            pos.x = x;
            continue;
        case '\t':
            pos.x += (m_char_width << 2);
            continue;
        }

        SDL_Rect src;
        src.w = m_char_width; src.h = m_char_height;

        const int sx = static_cast<int>(cur_char) % 16;
        const int sy = static_cast<int>(cur_char) >> 4;

        src.x = (sx * m_char_width) + sx;
        src.y = (sy * m_char_height) + sy;

        SDL_BlitSurface(Video::at(m_surface_index), &src, destination, &pos);
        pos.x += m_char_width;
    }
}
