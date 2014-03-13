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
#include "main.h"
#include "Video.h"
#include "Audio.h"
#include "game.h"
#include "state.h"


/*
 *  constants
 *
 */


const int DEFAULT_WIDTH = 640;
const int DEFAULT_HEIGHT = 480;
const int DEFAULT_BPP = 16;


/*
 *  functions
 *
 */


void sighandler(int signum)
{
    ENTER_FUNCTION(sighandler);

    switch(signum)
    {
    case SIGSEGV:
        std::cerr << "Segmentation Fault caught, exiting cleanly..." << std::endl;
        Callstack::dump(std::cerr);
        game_shutdown();
        exit(1);
    case SIGINT:
        std::cout << "Interrupt caught, exiting cleanly..." << std::endl;
        game_shutdown();
        exit(0);
    default:
        std::cout << "Unknown signal caught, continuing anyway..." << std::endl;
    }
}


inline void print_usage()
{
    ENTER_FUNCTION(print_usage);

    std::cout << "Usage: skratch [options]" << std::endl << std::endl
            << "Options:" << std::endl
            << "-width\t\tSet the window width" << std::endl
            << "-height\t\tSet the window height" << std::endl
            << "-bpp\t\tSet the window depth" << std::endl
            << "-fullscreen\tRun in fullscreen mode" << std::endl
            << "-nomusic\tTurn music off" << std::endl
            << "-nosound\tTurn sound off" << std::endl
            << "--help\t\tPrint this message" << std::endl << std::endl;
}


void process_arguments(const int argc, char* const argv[], State* const state)
{
    ENTER_FUNCTION(process_arguments);

    for(int i=1; i<argc; ++i) {
        if(!std::strcmp(argv[i], "-width")) {
            if(argc <= i+1) {
                std::cerr << "Expected argument to -width option" << std::endl;
                exit(1);
            }
            state->video_state.width = std::atoi(argv[++i]);
        } else if(!std::strcmp(argv[i], "-height")) {
            if(argc <= i+1) {
                std::cerr << "Expected argument to -height option" << std::endl;
                exit(1);
            }
            state->video_state.height = std::atoi(argv[++i]);
        } else if(!std::strcmp(argv[i], "-bpp")) {
            if(argc <= i+1) {
                std::cerr << "Expected argument to -bpp option" << std::endl;
                exit(1);
            }
            state->video_state.bpp = std::atoi(argv[++i]);
        } else if(!std::strcmp(argv[i], "-nomusic")) state->audio_state.music = false;
        else if(!std::strcmp(argv[i], "-nosound")) state->audio_state.sounds = false;
        else if(!std::strcmp(argv[i], "-fullscreen")) state->fullscreen = true;
        else if(!std::strcmp(argv[i], "-window")) state->fullscreen = false;
        else if(!std::strcmp(argv[i], "--help")) {
            print_usage();
            exit(0);
        } else std::cout << "Unknown argument '" << argv[i] << "'" << std::endl;
    }
}


inline void default_state(State* state)
{
    ENTER_FUNCTION(default_state);

    state->game_state = Menu;

    state->video_state.width  = DEFAULT_WIDTH;
    state->video_state.height = DEFAULT_HEIGHT;
    state->video_state.bpp    = DEFAULT_BPP;
}


int main(int argc, char* argv[])
{
    ENTER_FUNCTION(main);

#if defined WIN32
    std::cout << "Detected Win32" << std::endl;
#elif defined __linux__
    std::cout << "Detected Linux" << std::endl;
#elif defined __APPLE__
    std::cout << "Detected OSX" << std::endl;
#else
    std::cout << "Detected unknown OS" << std::endl;
#endif

    State state;
    default_state(&state);

    process_arguments(argc, argv, &state);
    state.video_state.width_scale  = static_cast<float>(state.video_state.width) / DEFAULT_WIDTH;
    state.video_state.height_scale = static_cast<float>(state.video_state.height) / DEFAULT_HEIGHT;

    std::cout << "Setting signal handlers..." << std::endl;
#if defined WIN32
    signal(SIGSEGV, sighandler);
    signal(SIGINT, sighandler);
#else
    struct sigaction signal_action;
    signal_action.sa_handler = sighandler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;

    sigaction(SIGSEGV, &signal_action, NULL);
    sigaction(SIGINT, &signal_action, NULL);
#endif

    std::auto_ptr<Video> video;
    try {
        std::auto_ptr<Video> v(new Video);
        video = v;
    } catch(Video::VideoException& e) {
        std::cerr << "Video initialization failed: " << e.what() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::auto_ptr<Audio> audio;
    try {
        std::auto_ptr<Audio> a(new Audio);
        audio = a;
    } catch(Audio::AudioException& e) {
        std::cerr << "Audio initialization failed: " << e.what() << std::endl;
        SDL_Quit();
        return 1;
    }

    if(!game_init(&state, *video, *audio)) {
        std::cerr << "Game initialization failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    game_run(&state);

    game_shutdown(&state);

#if 0
    Callstack::dump(std::cout);
#endif
    return 0;
}


TrigLookup::TrigLookup()
{
    ENTER_FUNCTION(TrigLookup::TrigLookup);

/*#if defined WIN32
    // this is from the math.h header in VC.NET
    #define M_PI 3.14159265358979323846
#endif*/

    for(int angle=0; angle < 360; ++angle) {
        const double theta = (angle * M_PI) / 180;

        sine[angle] = static_cast<float>(std::sin(theta));
        cosine[angle] = static_cast<float>(std::cos(theta));
        tangent[angle] = static_cast<float>(std::tan(theta));
    }
}
