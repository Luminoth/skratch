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
#include "menu.h"
#include "Video.h"
#include "Font.h"
#include "Timer.h"
#include "state.h"
#include "main.h"


/*
 *  typedefs
 *
 */


typedef void(*MenuFunction)(State*);


/*
 *  MenuOption class
 *
 */


class MenuOption
{
public:
    MenuOption(const std::string& text, int pixel_x, int pixel_y, int pixel_width, int pixel_height, MenuFunction function, State* state)
        : m_text(text), m_x(pixel_x), m_y(pixel_y), m_width(pixel_width), m_height(pixel_height), m_function(function), m_state(state)
    {
    }

public:
    // returns true if the mouse is inside the option
    bool mouse_in(int mouse_x, int mouse_y) const
    {
        return (mouse_x >= m_x && mouse_x <= (m_x + m_width) && mouse_y >= m_y && mouse_y <= (m_y + m_height));
    }

    void exec() const
    {
        if(m_function) m_function(m_state);
    }

public:
    const std::string& text() const { return m_text; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    std::string m_text;
    int m_x, m_y, m_width, m_height;

    MenuFunction m_function;
    State* m_state;
};


/*
 *  constants
 *
 */


const std::string MENU_FONT_FILENAME(DATADIR "/images/menufont.tga");
const int TITLE_OPTION_SPACING = 15;
const int OPTION_SPACING = 5;


/*
 *  external functions
 *
 */


/* FIXME: this probably isn't the best way to do this... */
extern void new_game(State* const state);
extern void exit_game(State* const state);


/*
 *  globals
 *
 */


Font g_menu_font;


/*
 *  functions
 *
 */


void main_menu(State* const state)
{
    ENTER_FUNCTION(main_menu);

    if(SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE) SDL_ShowCursor(SDL_ENABLE);
    if(!g_menu_font.loaded()) g_menu_font.load(MENU_FONT_FILENAME, state->video_state);

    state->game_state = Menu;
    state->menu_type = MainMenu;
    state->menu_state = TitleMenu;
}


void game_menu(State* const state)
{
    ENTER_FUNCTION(game_menu);

    if(SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE) SDL_ShowCursor(SDL_ENABLE);
    if(!g_menu_font.loaded()) g_menu_font.load(MENU_FONT_FILENAME, state->video_state);

    state->game_state = Menu;
    state->menu_type = GameMenu;
    state->menu_state = TitleMenu;
}


void to_options(State* const state)
{
    state->menu_state = OptionsMenu;
}


void to_title(State* const state)
{
    state->menu_state = TitleMenu;
}


void to_video(State* const state)
{
    state->menu_state = VideoMenu;
}


void to_audio(State* const state)
{
    state->menu_state = AudioMenu;
}


void build_title_option_list(State* const state, std::list<MenuOption>* const option_list)
{
    ENTER_FUNCTION(build_title_option_list);

    const int window_width = Video::window_width();
    const int font_width   = g_menu_font.char_width();
    const int font_height  = g_menu_font.char_height();

    std::string current_text(WINDOW_TITLE);
    int current_y = 10;
    int current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, NULL, NULL));

    current_text = "New Game";
    current_y += TITLE_OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &new_game, state));

    current_text = "Options";
    current_y += OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &to_options, state));

    current_text = "Exit";
    current_y += OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &exit_game, state));
}


void build_options_option_list(State* const state, std::list<MenuOption>* const option_list)
{
    ENTER_FUNCTION(build_options_option_list);

    const int window_width = Video::window_width();
    const int font_width   = g_menu_font.char_width();
    const int font_height  = g_menu_font.char_height();

    std::string current_text("Options");
    int current_y = 10;
    int current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, NULL, NULL));

    current_text = "Video";
    current_y += TITLE_OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &to_video, state));

    current_text = "Audio";
    current_y += OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &to_audio, state));

    current_text = "Back";
    current_y += OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &to_title, state));
}


void build_video_option_list(State* const state, std::list<MenuOption>* const option_list)
{
    ENTER_FUNCTION(build_video_option_list);

    const int window_width = Video::window_width();
    const int font_width   = g_menu_font.char_width();
    const int font_height  = g_menu_font.char_height();

    std::string current_text("Video");
    int current_y = 10;
    int current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, NULL, NULL));

    current_text = "Back";
    current_y += TITLE_OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &to_options, state));
}


void build_audio_option_list(State* const state, std::list<MenuOption>* const option_list)
{
    ENTER_FUNCTION(build_audio_option_list);

    const int window_width = Video::window_width();
    const int font_width   = g_menu_font.char_width();
    const int font_height  = g_menu_font.char_height();

    std::string current_text("Audio");
    int current_y = 10;
    int current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, NULL, NULL));

    current_text = "Back";
    current_y += TITLE_OPTION_SPACING + font_height;
    current_x = (window_width >> 1) - static_cast<int>((current_text.length() / 2.0f) * font_width);
    option_list->push_back(MenuOption(current_text, current_x, current_y, static_cast<int>(current_text.length() * font_width), font_height, &to_options, state));
}


void render_option_list(InputState* const input_state, std::list<MenuOption>* const option_list, const Timer& timer, bool fps)
{
    ENTER_FUNCTION(render_option_list);

    Video::clear_window();

    for(std::list<MenuOption>::const_iterator it = option_list->begin(); it != option_list->end(); ++it)
        Video::render_text(g_menu_font, it->text(), it->x(), it->y());

    if(fps) {
        char text[32];
        snprintf(text, 32, "FPS: %d", timer.fps());
        Video::render_text(g_menu_font, text, 0, 0);
    }

    Video::flip();
}


void handle_main_menu(State* const state)
{
    ENTER_FUNCTION(handle_main_menu);

/* FIXME: maintain the option list through frames if we can */

    std::list<MenuOption> option_list;

    switch(state->menu_state)
    {
    case TitleMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) exit_game(state);
        else if(state->input_state.keystate[SDLK_RETURN]) new_game(state);
        else {
            build_title_option_list(state, &option_list);
            render_option_list(&state->input_state, &option_list, (*state->timer), state->fps);
        }
        break;
    case OptionsMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) state->menu_state = TitleMenu;
        else {
            build_options_option_list(state, &option_list);
            render_option_list(&state->input_state, &option_list, (*state->timer), state->fps);
        }
        break;
    case VideoMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) state->menu_state = OptionsMenu;
        else {
            build_video_option_list(state, &option_list);
            render_option_list(&state->input_state, &option_list, (*state->timer), state->fps);
        }
        break;
    case AudioMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) state->menu_state = OptionsMenu;
        else {
            build_audio_option_list(state, &option_list);
            render_option_list(&state->input_state, &option_list, (*state->timer), state->fps);
        }
        break;
    default:
        std::cerr << "Invalid main menu state" << std::endl;
    }

    for(std::list<MenuOption>::const_iterator it = option_list.begin(); it != option_list.end(); ++it) {
        if(it->mouse_in(state->input_state.mouse_x, state->input_state.mouse_y) && state->input_state.mouse_buttons[0]) {
            it->exec();
            state->input_state.mouse_buttons[0] = false;
        }
    }
}


void handle_game_menu(State* const state)
{
    ENTER_FUNCTION(handle_game_menu);

    switch(state->menu_state)
    {
    case TitleMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) state->game_state = Running;
        break;
    case OptionsMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) state->menu_state = TitleMenu;
        break;
    case VideoMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) state->menu_state = OptionsMenu;
        break;
    case AudioMenu:
        if(state->input_state.keystate[SDLK_ESCAPE]) state->menu_state = OptionsMenu;
        break;
    default:
        std::cerr << "Invalid main menu state" << std::endl;
    }
}


void handle_menu(State* const state)
{
    ENTER_FUNCTION(handle_menu);

    if(state->game_state != Menu) return;

    if(state->menu_type == MainMenu) handle_main_menu(state);
    else if(state->menu_type == GameMenu) handle_game_menu(state);
    else std::cerr << "Bad menu type in handle_menu()" << std::endl;
}
