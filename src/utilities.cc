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

#include <cstdlib>
#include <iostream>

#if defined WIN32
    #include <direct.h>
    #define mkdir(s, m) _mkdir(s)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include <grp.h>
    #include <unistd.h>
#endif

#include "utilities.h"


/*
 *  cross-platform functions
 *
 */


void create_path(const std::string& path, unsigned int mode)
{
    unsigned int i=0;
    for(i=0; i<path.size(); ++i) {
        if(path[i] == '/' || path[i] == '\\')
            mkdir(path.substr(0, i).c_str(), mode);
    }
    if(path[path.length()-1] != '/' || path[i] == '\\')
        mkdir(path.c_str(), mode);
}


/*
 *  non-cross-platform functions
 *
 */


#if !defined WIN32
std::string get_prog_dir(std::string how_ran)
{
    char buffer[1024];
    if(!getcwd(buffer, 1024))
        return std::string();
    std::string cwd(buffer);

    while(how_ran[0] == '.') {
        int i = 0;
        while(how_ran.substr(i, 3) == "../") {
            int pos = cwd.rfind('/');
            cwd.erase(pos);
            how_ran.erase(0, 3);
        }

        while(how_ran.substr(0, 2) == "./")
            how_ran.erase(0, 2);
    }
    std::string ret(cwd + std::string("/") + how_ran);

    int pos = ret.rfind('/');
    ret.erase(pos);
    pos = ret.rfind('/');
    ret.erase(pos);
    return ret;
}


uid_t getuserid(const std::string& username)
{
    struct passwd* ent = getpwnam(username.c_str());
    if(!ent) {
        std::cerr << "Could not get id of user " << username << std::endl;
        exit(1);
    }
    return ent->pw_uid;
}


gid_t getgroupid(const std::string& groupname)
{
    struct group* grp = getgrnam(groupname.c_str());
    if(!grp) {
        std::cerr << "Could not get id of group " << groupname << std::endl;
        exit(1);
    }
    return grp->gr_gid;
}
#endif
