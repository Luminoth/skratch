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

#if !defined SHARED_H
#define SHARED_H


/*
==========
All standard and SDL headers are collected here.
All files that need a standard or SDL header should just include this one.
Add any needed standard or SDL header into this one rather than into the source/header file.

Also some good constants and portability tools are collected here...
==========
*/


#if defined WIN32
    #define _USE_MATH_DEFINES
#endif

#include <stdexcept>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstddef>
#include <ctime>
#include <cmath>
#include <cctype>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <csignal>
#include <climits>
#include <cfloat>
#include <typeinfo>
#include <algorithm>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <deque>
#include <stack>
#include <bitset>
#include <memory>
#include <utility>
#include <iostream>
#include <fstream>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>

#if !defined WIN32
    #include <sys/time.h>
    #include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

/* NOTE: do NOT add any unnecessary non-standard headers here! */
#include "Callstack.h"
#include "Vector.h"
#include "utilities.h"


/*
 *  pragmas
 *
 */


#if defined WIN32
    #pragma warning(disable : 4290)
#endif


/*
 *  function redefinitions
 *
 */


#if defined WIN32
    #define snprintf _snprintf
#endif


/*
 *  constants
 *
 */


#define VERSION "0.2.2"
#define WINDOW_TITLE "Scratch: Sk8er Kat v" VERSION

/* NOTE: these should be modified at compile time to the proper paths */
#define PREFIX "."
#define DATADIR PREFIX "/data"


/*
 *  typedefs
 *
 */


typedef unsigned char byte;


#endif
