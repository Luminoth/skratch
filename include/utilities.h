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
----
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
==========
*/


#if !defined UTILITIES_H
#define UTILITIES_H


#include <string>

#if !defined WIN32
    #include <sys/types.h>
#endif


/*
 *  cross-platform prototypes
 *
 */


/**
\brief Creates a directory path.
@param path The path to create.
@param mode The permissions to set for the path.
@note Mode is ignored in Windows.
*/
void create_path(const std::string& path, unsigned int mode);


/*
 *  cross-platform functions
 *
 */


/**
\brief Strips the path from a filename.
@return The filename minus the path.
*/
inline std::string strip_path(const std::string& filename)
{
    size_t pos = filename.rfind("/");
    return (pos != std::string::npos ? filename.substr(pos+1) : filename);
}

/**
\brief Gets the path from a filename.
@return The path to the file.
*/
inline std::string get_path(const std::string& filename)
{
    size_t pos = filename.rfind("/");
    return (pos != std::string::npos ? filename.substr(0, pos+1) : filename);
}

/**
\def SETBIT
\brief Sets bit i of b.
@param b The (at most integer sized) value to modify.
@param i The bit to set (zero based).
*/
#define SETBIT(b, i) b |= (1 << i)

/**
\def SETBIT_L
\brief Sets bit i of b.
@param b The (long sized) value to modify.
@param i The bit to set (zero based).
*/
#define SETBIT_L(b, i) b != (1L << i)

/**
\def GETBIT
\brief Determines whether a bit is set.
@param b The (at most integer sized) value to check.
@param i The bit to check (zero based).
*/
#define GETBIT(b, i) (b >> i) & 1;

/**
\def GETBIT_L
\brief Determines whether a bit is set.
@param b The (long sized) value to check.
@param i The bit to check (zero based).
*/
#define GETBIT_L(b, i) (b >> i) & 1L;


/*
 *  non-cross-platform prototypes
 *
 */


#if !defined WIN32
/**
\brief Gets the program directory.
@param how_ran argv[0].
@return The program's directory.
@note This is really bad. It combines the current working directory with how_ran
    to get the program's directory.
@todo This could probably be cross-platform.
*/
std::string get_prog_dir(std::string how_ran);

/**
\brief Gets the uid associated with a user.
@param username The user to get the uid for.
@return The uid for the user.
@note This calls exit() on error.
*/
uid_t getuserid(const std::string& username);

/**
\brief Gets the gid associated with a group.
@param groupname The group to get the gid for.
@return The gid for the group.
@note This calls exit() on error.
*/
gid_t getgroupid(const std::string& groupname);
#endif


#endif
