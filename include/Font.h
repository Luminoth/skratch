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

#if !defined FONT_H
#define FONT_H


#include "shared.h"


struct VideoState;


class Font
{
private:
    enum
    {
        DefaultWidth  = 255,
        DefaultHeight = 255
    };

public:
    Font();

public:
    // loads a font from a fontmap
    bool load(const std::string& filename, const VideoState& video_state);

    // renders the text on the destination surface at position
    void render_text(const std::string& text, SDL_Surface* const destination, int x, int y) const;

public:
    bool loaded() const { return m_surface_index >= 0; }
    int char_width() const { return m_char_width; }
    int char_height() const { return m_char_height; }

private:
    int m_surface_index;

    int m_char_width;
    int m_char_height;
};


#endif
