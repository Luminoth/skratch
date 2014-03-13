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


#if !defined TIMER_H
#define TIMER_H


class Timer
{
public:
    Timer();

public:
    // updates the current time
    // this has to be called for the elapsed functions to work
    /* NOTE: the constructor makes the first call of this */
    void update();

    // returns the amount of time that's elapsed in milliseconds
    unsigned int elapsed_ms() const;

    // returns the amount of time that's elapsed in seconds
    float elapsed_sec() const;

    // returns the instantaneous fps (0 if we're just going too fast)
    int fps() const;

private:
    unsigned int m_current;
    unsigned int m_last;
};


#endif
