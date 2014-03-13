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

#if !defined MENU_H
#define MENU_H


struct State;


/*
 *  prototypes
 *
 */


/* NOTE: the following two functions turn the cursor on, so if you want it off later, you gotta turn it off */

// sets the game state to the main menu
void main_menu(State* const state);

// sets the game state to the game menu
void game_menu(State* const state);

// handles the menu based states
// does all the rendering and handles input through the InputState structure
void handle_menu(State* const state);


#endif
