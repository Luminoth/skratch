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


#if !defined VIDEO_H
#define VIDEO_H


#include "shared.h"


class Font;


/* NOTE: these must be set for proper noimage loading */
#define NOIMAGE_EXTENSION ".tga"
#define NOIMAGE DATADIR "/images/noimage" NOIMAGE_EXTENSION


class Video
{
private:
    struct Surface
    {
        std::string name;
        SDL_Surface* surface;
        int index;

        Surface(const std::string& n, SDL_Surface* const s, int i)
            : name(n), surface(s), index(i)
        {
        }

        friend bool operator==(const Surface& lhs, const std::string& rhs)
        {
            return lhs.name == rhs;
        }

    };

public:
    // color constants (determined by endianness)
    static const Uint32 RMASK;
    static const Uint32 GMASK;
    static const Uint32 BMASK;
    static const Uint32 AMASK;

public:
    // initializes/shuts down the subsystem
    static bool initialize();
    static void shutdown() throw();

    // these are right out of the SDL documentation, so look there for their behavior
    static Uint32 get_pixel(int index, int x, int y);
    static Uint32 get_pixel(SDL_Surface* const surface, int x, int y);
    static void put_pixel(int index, int x, int y, Uint32 pixel);
    static void put_pixel(SDL_Surface* const surface, int x, int y, Uint32 pixel);

    // loads an image from a file
    // returns the index of the image in the hash or -1 on error
    static int load_image(const std::string& filename);

    // adds a copy of the surface into the surface hash
    // returns the index of the new surface in the hash or -1 on error
    static int push_back(SDL_Surface* const surface, const std::string& name);

    // adds a copy of the surface to the hash
    // returns the index of the new surface in the hash or -1 on error
    static int copy_surface(int index, const std::string& name);
    static int copy_surface(SDL_Surface* const surface, const std::string& name);

    // returns the index of the scaled copy of the surface in the hash or -1 on error
    static int scale_surface(int index, int width, int height, const std::string& name);
    static int scale_surface(SDL_Surface* const surface, int width, int height, const std::string& name);

    // does scaling, but replaces the image at index
    static int scale_surface(int index, int width, int height);

    // assigns a new name to the surface at index
    static void rename_surface(int index, const std::string& name);

    // returns the index of the flipped copy of the surface in the hash or -1 on error
    static int flip_surface_horizontal(int index, const std::string& name);
    static int flip_surface_horizontal(SDL_Surface* const surface, const std::string& name);
    static int flip_surface_vertical(int index, const std::string& name);
    static int flip_surface_vertical(SDL_Surface* const surface, const std::string& name);
    static int flip_surface_vert_horiz(int index, const std::string& name);
    static int flip_surface_vert_horiz(SDL_Surface* const surface, const std::string& name);

    // renders an surface onto the window 
    static void render_surface(int index, SDL_Rect* const srcrect, SDL_Rect* const pos);
    static void render_surface(SDL_Surface* const src, SDL_Rect* const srcrect, SDL_Rect* const pos);

    // swaps two colors on a surface
    static void swap_color(int index, Uint8 from_r, Uint8 from_g, Uint8 from_b, Uint8 to_r, Uint8 to_g, Uint8 to_b);

    // sets the color key of the surface in the hash at index
    static void set_color_key(int index, Uint8 r, Uint8 g, Uint8 b);

    // returns the surface at index or NULL on error
    // tries to load the image if it's been unloaded
    static SDL_Surface* const at(int index);

    // unloads a(ll) surface(s) in the hash
    static void unload_surface(int index);
    static void unload_surface(const std::string& name);
    static void unload_surfaces() throw();

    // prints the surface vector info to an output stream
    static void print_surfaces(std::ostream& outfile);

    // creates the main window
    static bool create_window(int width, int height, int desired_bpp, bool fullscreen, const std::string& title);

    // clears the window
    static void clear_window();

    // renders text onto the window
    static void render_text(const Font& font, const std::string& text, int x, int y);

    // saves a capture of the window to filename
    static void screenshot(const std::string& filename);

    // flips the backbuffer
    static void flip();

    // shows the cursor
    static void show_cursor();

    // hides the cursor
    static void hide_cursor();

    // blits a rect onto the window
    static void blit_rect(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b);

public:
    // returns the window width/height/depth/flags
    static int window_width() { return window ? window->w : -1; }
    static int window_height() { return window ? window->h : -1; }
    static int window_depth() { return window ? static_cast<int>(window->format->BitsPerPixel) : -1; }
    static Uint32 window_flags() { return window ? window->flags : 0; }

private:
    static void get_flags();

private:
    static int surface_size() { return static_cast<int>(surface_vector.size()); }

private:
    static std::vector<Surface> surface_vector;

    static SDL_Surface* window;
    static Uint32 flags;

public:
    class VideoException : public std::exception
    {
    public:
        VideoException(const std::string& what) throw() : _what(what) { }
        virtual ~VideoException() throw() { }
        virtual const char* what() const throw() { return _what.c_str(); }
    private:
        std::string _what;
    };

public:
    // these initialize and shutdown the subsystem, respectively
    // this makes it easy to shutdown on application exit
    Video() throw(VideoException);
    ~Video() throw();

public:
    // prints the status of the subsystem to an output stream
    // this is a good chunk of info, so be sure it's what you want
    friend std::ostream& operator<<(std::ostream& lhs, const Video& rhs);
};


#endif
