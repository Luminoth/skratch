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
#include "Video.h"
#include "Font.h"


/*
 *  Video constants
 *
 */


/* NOTE: these are setup to match what SDL_image calculates */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const Uint32 Video::RMASK = 0x000000ff;
    const Uint32 Video::GMASK = 0x0000ff00;
    const Uint32 Video::BMASK = 0xff000000;
    const Uint32 Video::AMASK = 0x00000000;
#else
    const Uint32 Video::RMASK = 0x00ff0000;
    const Uint32 Video::GMASK = 0x0000ff00;
    const Uint32 Video::BMASK = 0x000000ff;
    const Uint32 Video::AMASK = 0x00000000;
#endif


/*
 *  Video class variables
 *
 */


std::vector<Video::Surface> Video::surface_vector;

SDL_Surface* Video::window = NULL;
Uint32 Video::flags = 0;


/*
 *  Video class functions
 *
 */


bool Video::initialize()
{
    ENTER_FUNCTION(Video::initialize);

    return (SDL_Init(SDL_INIT_VIDEO) >= 0);
}


void Video::shutdown() throw()
{
    ENTER_FUNCTION(Video::shutdown);

    unload_surfaces();
    if(SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


Uint32 Video::get_pixel(int index, int x, int y)
{
    if(index >= surface_size() || index < 0) return 0;
    return get_pixel(surface_vector[index].surface, x, y);
}


Uint32 Video::get_pixel(SDL_Surface* const surface, int x, int y)
{
    if(!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) return 0;

    const int bpp = surface->format->BytesPerPixel;

    /* Here p is the address to the pixel we want to retrieve */
    Uint8* p = reinterpret_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return *p;
    case 2:
        return *(reinterpret_cast<Uint16*>(p));
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) return p[0] << 16 | p[1] << 8 | p[2];
        else return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(reinterpret_cast<Uint32*>(p));
    default:
        return 0;
    }
}


void Video::put_pixel(int index, int x, int y, Uint32 pixel)
{
    if(index >= surface_size() || index < 0) return;
    put_pixel(surface_vector[index].surface, x, y, pixel);
}


void Video::put_pixel(SDL_Surface* const surface, int x, int y, Uint32 pixel)
{
    if(!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;

    const int bpp = surface->format->BytesPerPixel;

    /* Here p is the address to the pixel we want to set */
    Uint8* p = reinterpret_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        *p = pixel;
        break;
    case 2:
        *(reinterpret_cast<Uint16*>(p)) = pixel;
        break;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(reinterpret_cast<Uint32*>(p)) = pixel;
        break;
    }
}


int Video::load_image(const std::string& filename)
{
    ENTER_FUNCTION(Video::load_image);

    if(filename.empty()) return -1;

    // only load it if we haven't already
    std::vector<Surface>::iterator it = std::find(surface_vector.begin(), surface_vector.end(), filename);
    if(it != surface_vector.end()) {
        if(it->surface) return it->index;
        else {
            // try to re-load the image
            SDL_Surface* surface = IMG_Load(filename.c_str());
            if(!surface) surface = IMG_Load(NOIMAGE);
            if(!surface) return -1;

            it->surface = SDL_DisplayFormat(surface);
            SDL_FreeSurface(surface);
            return it->index;
        }
    }

    // try to load the image
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if(!surface) surface = IMG_Load(NOIMAGE);
    if(!surface) return -1;

    surface_vector.push_back(Surface(filename, SDL_DisplayFormat(surface), surface_size()));
    SDL_FreeSurface(surface);
    return surface_size() - 1;
}


int Video::push_back(SDL_Surface* const surface, const std::string& name)
{
    ENTER_FUNCTION(Video::push_back);

    // reset the surface if we already have it
    std::vector<Surface>::iterator it = std::find(surface_vector.begin(), surface_vector.end(), name);
    if(it != surface_vector.end()) {
        if(it->surface) SDL_FreeSurface(it->surface);
        it->surface = surface;
        return it->index;
    }

    // add the surface
    surface_vector.push_back(Surface(name, surface, surface_size()));
    return surface_size() - 1;
}


int Video::copy_surface(int index, const std::string& name)
{
    ENTER_FUNCTION(Video::copy_surface);

    if(index >= surface_size() || index < 0) return -1;
    return copy_surface(surface_vector[index].surface, name);
}


int Video::copy_surface(SDL_Surface* const surface, const std::string& name)
{
    ENTER_FUNCTION(Video::copy_surface);

    if(!surface) return -1;

/* FIXME: there a faster way to do this? */

    SDL_Surface* surf = SDL_CreateRGBSurface(surface->flags, surface->w, surface->h, surface->format->BitsPerPixel, RMASK, GMASK, BMASK, AMASK);
    if(surf) {
        if(SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
        if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

            for(int y=0; y<surface->h; ++y)
                for(int x=0; x<surface->w; ++x)
                    put_pixel(surf, x, y, get_pixel(surface, x, y));

        if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        if(SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);

        surface_vector.push_back(Surface(name, SDL_DisplayFormat(surf), surface_size()));
        SDL_FreeSurface(surf);
        return surface_size() - 1;
    }
    return -1;
}


int Video::scale_surface(int index, int width, int height, const std::string& name)
{
    ENTER_FUNCTION(Video::scale_surface);

    if(index >= surface_size() || index < 0) return -1;
    return scale_surface(surface_vector[index].surface, width, height, name);
}


int Video::scale_surface(SDL_Surface* const surface, int width, int height, const std::string& name)
{
    ENTER_FUNCTION(Video::scale_surface);

/*
This uses a modified bi-linear filtering algorithm from
"Tricks of the Windows Game Programming Gurus" (Lamothe, 1999 Sams, p. 371)
*/

    if(!surface || width <= 0 || height <= 0) return -1;

    // don't scale if we don't have to
    if(width == surface->w && height == surface->h) return copy_surface(surface, name);

    SDL_Surface* surf = SDL_CreateRGBSurface(surface->flags, width, height, surface->format->BitsPerPixel, RMASK, GMASK, BMASK, AMASK);
    if(surf) {
        const float sample_x_rate = static_cast<float>(surface->w) / width;
        const float sample_y_rate = static_cast<float>(surface->h) / height;

        float sample_x = 0.0f;
        float sample_y = 0.0f;

        if(SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
        if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

            for(int y=0; y<height; ++y) {
                sample_x = 0.0f;

                for(int x=0; x<width; ++x) {
                    put_pixel(surf, x, y, get_pixel(surface, static_cast<int>(sample_x), static_cast<int>(sample_y)));
                    sample_x += sample_x_rate;
                }
                sample_y += sample_y_rate;
            }

        if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        if(SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);

        surface_vector.push_back(Surface(name, SDL_DisplayFormat(surf), surface_size()));
        SDL_FreeSurface(surf);
        return surface_size() - 1;
    }
    return -1;
}


int Video::scale_surface(int index, int width, int height)
{
    ENTER_FUNCTION(Video::scale_surface);

/*
(1) add a second entry to the vector with the same name as the original
(2) free the old image
(3) copy the new image pointer to the old
(4) remove the the old entry from the vector
*/

    if(index >= surface_size() || index < 0) return -1;
    int temp_index = scale_surface(surface_vector[index].surface, width, height, surface_vector[index].name);
    if(temp_index < 0) return -1;

    SDL_FreeSurface(surface_vector[index].surface);
    surface_vector[index].surface = surface_vector[temp_index].surface;

    surface_vector.pop_back();
    return index;
}


void Video::rename_surface(int index, const std::string& name)
{
    ENTER_FUNCTION(Video::rename_surface);

    if(index >= surface_size() || index < 0) return;
    surface_vector[index].name = name;
}


int Video::flip_surface_horizontal(int index, const std::string& name)
{
    ENTER_FUNCTION(Video::flip_surface_horizontal);

    if(index >= surface_size() || index < 0) return -1;
    return flip_surface_horizontal(surface_vector[index].surface, name);
}


int Video::flip_surface_horizontal(SDL_Surface* const surface, const std::string& name)
{
    ENTER_FUNCTION(Video::flip_surface_horizontal);

    if(!surface) return -1;

/* FIXME: is there a faster way to do this? */

    SDL_Surface* surf = SDL_CreateRGBSurface(surface->flags, surface->w, surface->h, surface->format->BitsPerPixel, RMASK, GMASK, BMASK, AMASK);
    if(surf) {
        if(SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
        if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

            for(int y=0; y<surface->h; ++y)
                for(int x=0; x<surface->w; ++x)
                    put_pixel(surf, (surf->w - x) - 1, y, get_pixel(surface, x, y));

        if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        if(SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);

        surface_vector.push_back(Surface(name, SDL_DisplayFormat(surf), surface_size()));
        SDL_FreeSurface(surf);
        return surface_size() - 1;
    }
    return -1;
}


int Video::flip_surface_vertical(int index, const std::string& name)
{
    ENTER_FUNCTION(Video::flip_surface_vertical);

    if(index >= surface_size() || index < 0) return -1;
    return flip_surface_vertical(surface_vector[index].surface, name);
}


int Video::flip_surface_vertical(SDL_Surface* const surface, const std::string& name)
{
    ENTER_FUNCTION(Video::flip_surface_vertical);

    if(!surface) return -1;

/* FIXME: is there a faster way to do this? */

    SDL_Surface* surf = SDL_CreateRGBSurface(surface->flags, surface->w, surface->h, surface->format->BitsPerPixel, RMASK, GMASK, BMASK, AMASK);
    if(surf) {
        if(SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
        if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

            for(int y=0; y<surface->h; ++y)
                for(int x=0; x<surface->w; ++x)
                    put_pixel(surf, x, (surf->h - y) - 1, get_pixel(surface, x, y));

        if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        if(SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);

        surface_vector.push_back(Surface(name, SDL_DisplayFormat(surf), surface_size()));
        SDL_FreeSurface(surf);
        return surface_size() - 1;
    }
    return -1;
}


int Video::flip_surface_vert_horiz(int index, const std::string& name)
{
    ENTER_FUNCTION(Video::flip_surface_vert_horiz);

    if(index >= surface_size() || index < 0) return -1;
    return flip_surface_vert_horiz(surface_vector[index].surface, name);
}


int Video::flip_surface_vert_horiz(SDL_Surface* const surface, const std::string& name)
{
    ENTER_FUNCTION(Video::flip_surface_vert_horiz);

    if(!surface) return -1;

/* FIXME: is there a faster way to do this? */

    SDL_Surface* surf = SDL_CreateRGBSurface(surface->flags, surface->w, surface->h, surface->format->BitsPerPixel, RMASK, GMASK, BMASK, AMASK);
    if(surf) {
        if(SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
        if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

            for(int y=0; y<surface->h; ++y)
                for(int x=0; x<surface->w; ++x)
                    put_pixel(surf, (surf->w - x) - 1, (surf->h - y) - 1, get_pixel(surface, x, y));

        if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        if(SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);

        surface_vector.push_back(Surface(name, SDL_DisplayFormat(surf), surface_size()));
        SDL_FreeSurface(surf);
        return surface_size() - 1;
    }
    return -1;
}


void Video::render_surface(int index, SDL_Rect* const srcrect, SDL_Rect* const pos)
{
    ENTER_FUNCTION(Video::render_surface);

    if(index >= surface_size() || index < 0 || !surface_vector[index].surface) return;
    if(window) SDL_BlitSurface(surface_vector[index].surface, srcrect, window, pos);
}


void Video::render_surface(SDL_Surface* const src, SDL_Rect* const srcrect, SDL_Rect* const pos)
{
    ENTER_FUNCTION(Video::render_surface);

    if(!src) return;
    if(window) SDL_BlitSurface(src, srcrect, window, pos);
}


void Video::swap_color(int index, Uint8 from_r, Uint8 from_g, Uint8 from_b, Uint8 to_r, Uint8 to_g, Uint8 to_b)
{
    ENTER_FUNCTION(Video::swap_color);

    if(index >= surface_size() || index < 0 || !surface_vector[index].surface) return;

/* FIXME: is there a faster way to do this? */
/* FIXME: should we really swap in place? or return a copy? */

    SDL_Surface* surface = surface_vector[index].surface;
    Uint8 r, g, b;

    if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

        for(int y=0; y<surface->h; ++y) {
            for(int x=0; x<surface->w; ++x) {
                Uint32 pixel = get_pixel(index, x, y);
                SDL_GetRGB(pixel, surface->format, &r, &g, &b);

                if(r == from_r && g == from_g && b == from_b)
                    put_pixel(index, x, y, SDL_MapRGB(surface->format, to_r, to_g, to_b));
            }
        }

    if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
}


void Video::set_color_key(int index, Uint8 r, Uint8 g, Uint8 b)
{
    ENTER_FUNCTION(Video::set_color_key);

    if(index >= surface_size() || index < 0 || !surface_vector[index].surface) return;
    SDL_SetColorKey(surface_vector[index].surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface_vector[index].surface->format, r, g, b));
}


SDL_Surface* const Video::at(int index)
{
    ENTER_FUNCTION(Video::at);

    if(index >= surface_size() || index < 0) return NULL;

    if(!surface_vector[index].surface) {
        if(!surface_vector[index].name.empty()) {
            SDL_Surface* surface = IMG_Load(surface_vector[index].name.c_str());
            if(!surface) surface = IMG_Load(NOIMAGE);
            if(!surface) return NULL;
            surface_vector[index].surface = SDL_DisplayFormat(surface);
            SDL_FreeSurface(surface);
            return surface_vector[index].surface;
        }
        return NULL;
    }
    return surface_vector[index].surface;
}


void Video::unload_surface(int index)
{
    ENTER_FUNCTION(Video::unload_surface);

    if(index >= surface_size() || index < 0) return;

    // don't unload the noimage images
    if(std::string::npos != surface_vector[index].name.find("noimage")) return;

    if(surface_vector[index].surface) {
        SDL_FreeSurface(surface_vector[index].surface);
        surface_vector[index].surface = NULL;
    }
}


void Video::unload_surface(const std::string& name)
{
    ENTER_FUNCTION(Video::unload_surface);

    std::vector<Surface>::iterator it = std::find(surface_vector.begin(), surface_vector.end(), name);
    if(it != surface_vector.end()) unload_surface(it->index);
}


void Video::unload_surfaces() throw()
{
    ENTER_FUNCTION(Video::unload_surfaces);

    for(std::vector<Surface>::iterator it = surface_vector.begin(); it != surface_vector.end(); ++it) {
        if(it->surface) SDL_FreeSurface(it->surface);
        it->surface = NULL;
    }
}


void Video::print_surfaces(std::ostream& outfile)
{
    ENTER_FUNCTION(Video::print_surfaces);

    outfile << "I have " << surface_size() << " surfaces" << std::endl;
    for(std::vector<Surface>::const_iterator it = surface_vector.begin(); it != surface_vector.end(); ++it) {
        outfile << it->index << ": " << it->name << " ";

        if(!it->surface) outfile << "(NULL)" << std::endl;
        else outfile << "(Size: " << it->surface->w << "x" << it->surface->h << ")" << std::endl;
    }
}


/* FIXME: what if the window was already created? */
bool Video::create_window(int width, int height, int desired_bpp, bool fullscreen, const std::string& title)
{
    ENTER_FUNCTION(Video::create_window);

    const int bpp = SDL_VideoModeOK(width, height, desired_bpp, flags);
    if(!bpp) return false;

    std::cout << "Creating window '" << title << "': " << width << "x" << height << "x" << bpp << "..." << std::endl;

    if(fullscreen) flags |= SDL_FULLSCREEN;
    else flags &= ~SDL_FULLSCREEN;
    window = SDL_SetVideoMode(width, height, bpp, flags);

    SDL_WM_SetCaption(title.c_str(), NULL);
    return true;
}


void Video::clear_window()
{
    ENTER_FUNCTION(Video::clear_window);

    if(window) SDL_FillRect(window, NULL, SDL_MapRGB(window->format, 0, 0, 0));
}


void Video::render_text(const Font& font, const std::string& text, int x, int y)
{
    ENTER_FUNCTION(Video::render_text);

    if(window) font.render_text(text, window, x, y);
}


void Video::screenshot(const std::string& filename)
{
    ENTER_FUNCTION(Video::screenshot);

    if(window) SDL_SaveBMP(window, filename.c_str());
}


void Video::flip()
{
    ENTER_FUNCTION(Video::flip);

    if(window) SDL_Flip(window);
}


void Video::show_cursor()
{
    ENTER_FUNCTION(Video::show_cursor);

    if(SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE)
        SDL_ShowCursor(SDL_ENABLE);
}


void Video::hide_cursor()
{
    ENTER_FUNCTION(Video::hide_cursor);

    if(SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE)
        SDL_ShowCursor(SDL_DISABLE);
}


void Video::blit_rect(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b)
{
    ENTER_FUNCTION(Video::blit_rect);

    SDL_Rect rect;
    rect.x = x; rect.y = y;
    rect.w = width; rect.h = height;

    if(window) SDL_FillRect(window, &rect, SDL_MapRGB(window->format, r, g, b));
}


void Video::get_flags()
{
    ENTER_FUNCTION(Video::get_flags);

    flags = 0;

    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    if(info->hw_available) flags |= (SDL_HWSURFACE | SDL_DOUBLEBUF);
    else flags |= SDL_SWSURFACE;
}


/*
 *  Video methods
 *
 */


Video::Video() throw(Video::VideoException)
{
    ENTER_FUNCTION(Video::Video);

    if(!initialize())
        throw(VideoException(SDL_GetError()));
    get_flags();
}


Video::~Video() throw()
{
    ENTER_FUNCTION(Video::~Video);

    shutdown();
}


/*
 *  Video friend functions
 *
 */


std::ostream& operator<<(std::ostream& lhs, const Video& rhs)
{
    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    lhs << "Video subsystem information:" << std::endl;

    char buf[32];
    SDL_VideoDriverName(buf, 32);

    lhs << "\tDriver: " << buf << " (";
    if(info->hw_available) lhs << "Hardware)" << std::endl;
    else lhs << "Software)" << std::endl;

    if(Video::window) {
        lhs << "\tWindow width: " << Video::window_width() << std::endl
            << "\tWindow height: " << Video::window_height() << std::endl
            << "\tWindow depth: " << Video::window_depth() << std::endl
            << "\tFullscreen: ";
        if(Video::flags & SDL_FULLSCREEN) lhs << "Yes" << std::endl;
        else lhs << "No" << std::endl;
    } else lhs << "\tNo window information avaiable" << std::endl;

/* hardware specific info */

    lhs << "\tVideo memory: " << info->video_mem << " kilobytes";

#if defined DEBUG
    lhs << std::endl << std::endl << "\tHardware surfaces are ";
    if(info->hw_available) lhs << "available" << std::endl;
    else lhs << "not available" << std::endl;

    lhs << "\tHardware blits are ";
    if(info->blit_hw) lhs << "accelerated" << std::endl;
    else lhs << "not accelerated" << std::endl;

    lhs << "\tHardware colorkey blits are ";
    if(info->blit_hw_CC) lhs << "accelerated" << std::endl;
    else lhs << "not accelerated" << std::endl;

    lhs << "\tHardware alpha blits are ";
    if(info->blit_hw_A) lhs << "accelerated" << std::endl;
    else lhs << "not accelerated" << std::endl;

    lhs << "\tSoftware blits are ";
    if(info->blit_sw) lhs << "accelerated" << std::endl;
    else lhs << "not accelerated" << std::endl;

    lhs << "\tSoftware colorkey blits are ";
    if(info->blit_sw_CC) lhs << "accelerated" << std::endl;
    else lhs << "not accelerated" << std::endl;

    lhs << "\tSoftware alpha blits are ";
    if(info->blit_sw_A) lhs << "accelerated" << std::endl;
    else lhs << "not accelerated" << std::endl;

    lhs << "\tBlit fills are ";
    if(info->blit_fill) lhs << "accelerated" << std::endl;
    else lhs << "not accelerated";
#endif

    return lhs;
}
