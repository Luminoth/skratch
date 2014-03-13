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
#include "game.h"
#include "Video.h"
#include "Audio.h"
#include "Font.h"
#include "Timer.h"
#include "Skratch.h"
#include "World.h"
#include "menu.h"
#include "main.h"
#include "state.h"


/*
 *  constants
 *
 */


const std::string HUD_FONT_FILENAME(DATADIR "/images/hudfont.tga");


/*
 *  prototypes
 *
 */


void event_loop(State* const state);


/*
 *  globals
 *
 */


TrigLookup g_trig_lookup;


/*
 *  functions
 *
 */


bool create_window(const VideoState& video_state, bool fullscreen)
{
    ENTER_FUNCTION(create_window);

    const std::string title(WINDOW_TITLE);

    if(fullscreen) {
        std::cout << "Trying fullscreen mode..." << std::endl;
        if(!Video::create_window(video_state.width, video_state.height, video_state.bpp, true, title)) {
            std::cout << "Fullscreen mode failed (" << SDL_GetError() << "), trying windowed..." << std::endl;
            if(!Video::create_window(video_state.width, video_state.height, video_state.bpp, false, title)) return false;
        }
    } else {
        std::cout << "Trying windowed mode..." << std::endl;
        if(!Video::create_window(video_state.width, video_state.height, video_state.bpp, false, title)) return false;
    }
    return true;
}


bool game_init(State* const state, const Video& video, const Audio& audio)
{
    ENTER_FUNCTION(game_init);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    std::cout << "Detected big endian" << std::endl;
#else
    std::cout << "Detected little endian" << std::endl;
#endif

    srand(static_cast<unsigned int>(time(NULL)));

    if(!create_window(state->video_state, state->fullscreen)) return false;

    std::cout << std::endl << video << std::endl;

    if(!Audio::start()) return false;

    std::cout << std::endl << audio << std::endl;

    return true;
}


void game_run(State* const state)
{
    ENTER_FUNCTION(game_run);

    main_menu(state);

    Timer timer;
    state->timer = &timer;
    while(state->game_state != Quit) {
        event_loop(state);
        timer.update();
    }
}


void game_shutdown(State* const state)
{
    ENTER_FUNCTION(game_shutdown);

    Entity::free_entities();

    if(state->world) delete state->world;
    state->world = NULL;

    if(state->player_state.player) delete state->player_state.player;
    state->player_state.player = NULL;

    Audio::unload_all();
    Video::unload_surfaces();

    if(SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE) SDL_ShowCursor(SDL_ENABLE);
    SDL_Quit();
}


void game_shutdown()
{
    ENTER_FUNCTION(game_shutdown);

    if(SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE) SDL_ShowCursor(SDL_ENABLE);
    SDL_Quit();
}


/* this is used in the menu module */
void exit_game(State* const state)
{
    ENTER_FUNCTION(exit_game);

    state->game_state = Quit;
}


/* this is used in the menu module */
void new_game(State* const state)
{
    ENTER_FUNCTION(new_game);

    if(SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE) SDL_ShowCursor(SDL_DISABLE);

    state->game_state = Running;

    state->player_state.lives = 9;
    state->player_state.score = 0L;

    Entity::free_entities();

    if(state->player_state.player) delete state->player_state.player;
    state->player_state.player = new Skratch();
    state->player_state.player->load_media(state->video_state);

    if(state->world) delete state->world;
    state->world = new World();
    if(!state->world->load("level01", state->video_state, state->player_state.player)) {
        std::cerr << "Couldn't load intro world" << std::endl;
        exit_game(state);
    }
}


void restart_game(State* const state)
{
    ENTER_FUNCTION(restart_game);

    if(state->player_state.player) delete state->player_state.player;
    state->player_state.player = new Skratch();
    state->player_state.player->load_media(state->video_state);

    Entity::free_entities();

    if(state->world) delete state->world;
    state->world = new World();
    if(!state->world->load("level01", state->video_state, state->player_state.player)) {
        std::cerr << "Couldn't load intro world" << std::endl;
        exit_game(state);
    }
}


void render_hud(const PlayerState& player_state, const VideoState& video_state, const Timer& timer, bool fps, bool paused)
{
    ENTER_FUNCTION(render_hud);

    static Font hud_font;
    if(!hud_font.loaded()) hud_font.load(HUD_FONT_FILENAME, video_state);

    int cur_x=0, cur_y=0;

    char text[32];
    snprintf(text, 32, "Score: %ld", player_state.score);
    Video::render_text(hud_font, text, cur_x, cur_y);

    cur_y += hud_font.char_height();

    snprintf(text, 32, "Lives: %d", player_state.lives);
    Video::render_text(hud_font, text, cur_x, cur_y);

    if(fps) {
        cur_y += hud_font.char_height();
        snprintf(text, 32, "FPS: %d", timer.fps());
        Video::render_text(hud_font, text, cur_x, cur_y);
    }

    if(paused) {
        cur_y += hud_font.char_height();
        strncpy(text, "Paused", 32);
        Video::render_text(hud_font, text, cur_x, cur_y);
    }
}


void screenshot()
{
    ENTER_FUNCTION(screenshot);

    create_path(DATADIR "/screenshots", 0755);

    int cur_shot = 0;
    char path[1024];
    snprintf(path, 1024, DATADIR "/screenshots/skratch%d.bmp", cur_shot);

    struct stat buf;
    while(!stat(path, &buf))
        snprintf(path, 1024, DATADIR "/screenshots/skratch%d.bmp", ++cur_shot);
    Video::screenshot(path);
}


void handle_state(State* const state)
{
    ENTER_FUNCTION(handle_state);

    World* world = state->world;
    Skratch* skratch = state->player_state.player;

    std::bitset<World::CollisionSize> collisions;

    switch(state->game_state)
    {
    case Quit: break;
    case Menu:
        handle_menu(state);
        break;
    case Running:
        if(!state->paused) {
            Entity::sort();

            Entity::all_think(*world);
            skratch->think(state->video_state, state->input_state.keystate, *world);

            Entity::all_animate(state->timer->elapsed_sec(), *world);

            collisions = skratch->animate(state->timer->elapsed_sec(), world);
            if(collisions[World::FellOffWorld] || skratch->removable()) {
                if(state->player_state.lives) {
                    --state->player_state.lives;
                    restart_game(state);
                    return;
                } else {
std::cout << "You lose!" << std::endl;
exit_game(state);
                }
            } else if(collisions[World::EndWorld]) {
std::cout << "You win!" << std::endl;
exit_game(state);
            }
        }

        Video::clear_window();

        world->render();
        Entity::render_entities(*world);
        skratch->render(*world);

        render_hud(state->player_state, state->video_state, *(state->timer), state->fps, state->paused);

        if(state->input_state.keystate[SDLK_F11]) {
            screenshot();
            state->input_state.keystate[SDLK_F11] = false;
        }

        Entity::cleanup();
        Video::flip();
        break;
    case EndGame: break;
    default:
        std::cout << "Unknown game state" << std::endl;
        break;
    }
}


void event_loop(State* const state)
{
    ENTER_FUNCTION(event_loop);

    if(Quit == state->game_state) return;

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type)
        {
        case SDL_MOUSEMOTION:
            state->input_state.mouse_x = event.motion.x;
            state->input_state.mouse_y = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            state->input_state.mouse_x = event.button.x;
            state->input_state.mouse_y = event.button.y;

            switch(event.button.button)
            {
            case SDL_BUTTON_LEFT:
                state->input_state.mouse_buttons[0] = (event.button.state == SDL_PRESSED);
                break;
            case SDL_BUTTON_MIDDLE:
                state->input_state.mouse_buttons[1] = (event.button.state == SDL_PRESSED);
                break;
            case SDL_BUTTON_RIGHT:
                state->input_state.mouse_buttons[2] = (event.button.state == SDL_PRESSED);
                break;
            }
            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                if(Running == state->game_state) {
                    /*state->game_state = Menu;
                    state->menu_state = Title;
                    state->menu_type = GameMenu;*/

                    exit_game(state);
                } else if(EndGame == state->game_state) exit_game(state);
                else if(Story == state->game_state) new_game(state);
                break;
            case SDLK_q:
                if(Running == state->game_state) exit_game(state);
                break;
            default:
                if(EndGame == state->game_state) main_menu(state);
                state->input_state.keystate[event.key.keysym.sym] = false;
            }
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_a:
                if(Running == state->game_state) {
                    Audio::print_all(std::cout);
                    std::cout << std::endl;
                }
                break;
            case SDLK_e:
                if(Running == state->game_state) {
                    Entity::print_entities(std::cout);
                    std::cout << std::endl;
                }
                break;
            case SDLK_f:
                state->fps = !state->fps;
                break;
            case SDLK_p:
                if(Running == state->game_state) state->paused = !state->paused;
                break;
            case SDLK_s:
                if(Running == state->game_state) {
                    Video::print_surfaces(std::cout);
                    std::cout << std::endl;
                }
                break;
            default:
                state->input_state.keystate[event.key.keysym.sym] = true;
            }
            break;
        case SDL_QUIT:
            exit_game(state);
            break;
        default: break;
        }
    }
    handle_state(state);
}
