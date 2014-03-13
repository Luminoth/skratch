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


/*
compile with:

g++ -Wall main.cc -o editor
*/


/*
World map file format:

1 byte world width 1 byte world height\n
THE WORLD AS CHARACTERS, NEWLINE SEPERATED\n

Entity map file format:

THE ENTITIES AS CHARACTERS, NEWLINE SEPERATED\n
*/


#include <iostream>
#include <fstream>
#include <string>


struct WorldHeader
{
    char width;
    char height;
};


bool create_collision_map(const WorldHeader& header)
{
    std::cout << std::endl << "Creating collision map..." << std::endl;

    std::ofstream outfile("collision.map");
    if(!outfile) {
        std::cerr << "Couldn't create collision map file" << std::endl;
        return false;
    }

    std::cout << "Writing world header..." << std::endl;

    outfile.put(header.width);
    outfile.put(header.height);
    outfile.put('\n');

    std::cout << "Writing temp map..." << std::endl << std::endl;

    for(int y=0; y<header.height; y++) {
        for(int x=0; x<header.width; x++) outfile.put('0');
        outfile.put('\n');
    }
    outfile.clear(); outfile.close();
    return true;
}


bool verify_collision_map(const WorldHeader& header)
{
    struct WorldHeader scratch;

    std::cout << "Verifying world header..." << std::endl << std::endl;

    std::ifstream infile("collision.map");
    if(!infile) {
        std::cerr << "Couldn't open collision map file for verification" << std::endl;
        return false;
    }

    infile.get(scratch.width);
    infile.get(scratch.height);
    infile.clear(); infile.close();

    std::cout << "width = " << (int)header.width << " - " << (int)scratch.width << std::endl
            << "height = " << (int)header.height << " - " << (int)scratch.height << std::endl << std::endl;

    if(memcmp(&header, &scratch, sizeof(struct WorldHeader))) {
        std::cerr << "Mismatch!" << std::endl << std::endl;
        return false;
    }
    std::cout << "Looks good!" << std::endl << std::endl;
    return true;
}


bool create_texture_map(const WorldHeader& header)
{
    std::cout << std::endl << "Creating texture map..." << std::endl;

    std::ofstream outfile("texture.map");
    if(!outfile) {
        std::cerr << "Couldn't create texture map file" << std::endl;
        return false;
    }

    std::cout << "Writing temp map..." << std::endl << std::endl;

    for(int y=0; y<header.height; y++) {
        for(int x=0; x<header.width; x++) outfile.put('0');
        outfile.put('\n');
    }
    outfile.clear(); outfile.close();
    return true;
}


bool create_entity_map(const WorldHeader& header)
{
    std::cout << "Creating entity map..." << std::endl;

    std::ofstream outfile("entity.map");
    if(!outfile) {
        std::cerr << "Couldn't create entity map file" << std::endl;
        return false;
    }

    std::cout << "Writing temp map..." << std::endl << std::endl;

    for(int y=0; y<header.height; y++) {
        for(int x=0; x<header.width; x++) outfile.put('0');
        outfile.put('\n');
    }
    outfile.clear(); outfile.close();
    return true;
}


int main(int argc, char* argv[])
{
    struct WorldHeader header;

    std::cout << "Enter world width: " << std::flush;

    std::string scratch;
    getline(std::cin, scratch);
    header.width = atoi(scratch.c_str());

    std::cout << "Enter world height: " << std::flush;

    getline(std::cin, scratch);
    header.height = atoi(scratch.c_str());

    if(!create_collision_map(header)) return 1;
    if(!verify_collision_map(header)) return 1;
    if(!create_texture_map(header)) return 1;
    if(!create_entity_map(header)) return 1;

    return 0;
}
