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
#include "Timer.h"


/*
 *  Timer methods
 *
 */


Timer::Timer() : m_current(0), m_last(0)
{
    update();
}


void Timer::update()
{
    m_last = m_current;
    m_current = SDL_GetTicks();
}


unsigned int Timer::elapsed_ms() const
{
    return m_current - m_last;
}


float Timer::elapsed_sec() const
{
    return elapsed_ms() / 1000.0f;
}


int Timer::fps() const
{
    const int ms = elapsed_ms();
    return ms ? 1000 / ms : 0;
}
