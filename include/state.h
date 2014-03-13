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

#if !defined STATE_H
#define STATE_H


#include "shared.h"


class Timer;
class World;
class Skratch;


/*
 *  state constants
 *
 */


enum GameState
{
    Menu,       /* generic menu state */
    Story,      /* generic story state */
    Running,    /* we're in the world playing the game */
    EndGame,    /* generic end game state */
    Quit
};


enum MenuState
{
    None,
    TitleMenu,
    OptionsMenu,
    VideoMenu,
    AudioMenu
};


enum MenuType
{
    NoMenu,
    MainMenu,   /* main menu */
    GameMenu    /* in-game menu */
};


/*
 *  state structures
 *
 */


struct PlayerState
{
    Skratch* player;

    int lives;
    long score;

    PlayerState() : player(NULL), lives(0), score(0L)
    {
    }
};


struct VideoState
{
    int width, height, bpp;             /* these are used for creating the window, actual width/height/depth is in the Video class */
    float width_scale, height_scale;    /* these are how much to scale images from the default size */

    VideoState() : width(0), height(0), bpp(0), width_scale(0.0f), height_scale(0.0f)
    {
    }
};


struct AudioState
{
    int volume; /* unused at the moment */
    bool sounds, music;

    AudioState() : volume(0), sounds(true), music(true)
    {
    }
};


struct InputState
{
    bool keystate[SDLK_LAST];
    bool mouse_buttons[3];
    int mouse_x, mouse_y;

    InputState() : mouse_x(0), mouse_y(0)
    {
        memset(keystate, 0, SDLK_LAST*sizeof(bool));
        memset(mouse_buttons, 0, 3*sizeof(bool));
    }
};


struct State
{
    GameState game_state;
    bool paused;
    bool fps;
    bool fullscreen;

    struct PlayerState player_state;
    struct VideoState video_state;
    struct AudioState audio_state;
    struct InputState input_state;

    MenuState menu_state;
    MenuType menu_type;

    World* world;
    Timer* timer;

    State() : game_state(Quit), paused(false), fps(/*false*/true), fullscreen(false), menu_state(None), menu_type(NoMenu), world(NULL), timer(NULL)
    {
    }
};


#endif
