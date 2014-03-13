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

#if !defined GAME_H
#define GAME_H


class Video;
class Audio;
struct State;


/*
 *  prototypes
 *
 */


bool game_init(State* const state, const Video& video, const Audio& audio);
void game_run(State* const state);
void game_shutdown(State* const state);

/* this is a special case shutdown (it misses a lot of cleanup, but gets the job done). only use in an emergency */
void game_shutdown();


#endif
