/**
\file Audio.cc
\author Shane Lillie
\brief Audio subsystem module source.

\verbatim
Copyright 2002-2003 Energon Software

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
\endverbatim
*/


#include "Audio.h"


/*
 *  Audio class variables
 *
 */


std::vector<Audio::Sound> Audio::sound_vector;
std::vector<Audio::Music> Audio::music_vector;

int Audio::buffer_size = 512;
int Audio::frequency   = MIX_DEFAULT_FREQUENCY;
int Audio::channels    = 2;
Uint16 Audio::format   = MIX_DEFAULT_FORMAT;


/*
 *  Audio class functions
 *
 */


bool Audio::initialize()
{
    ENTER_FUNCTION(Audio::initialize);

    return (SDL_Init(SDL_INIT_AUDIO) >= 0);
}


void Audio::shutdown() throw()
{
    ENTER_FUNCTION(Audio::shutdown);

    stop();

    unload_all();

    if(SDL_WasInit(SDL_INIT_AUDIO))
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


int Audio::push_back(Mix_Chunk* const sound, const std::string& name)
{
    ENTER_FUNCTION(Audio::push_back);

    // overwrite any previously loaded sound
    std::vector<Sound>::iterator it = std::find(sound_vector.begin(), sound_vector.end(), name);
    if(it != sound_vector.end()) {
        if(it->chunk) Mix_FreeChunk(it->chunk);
        it->chunk = sound;
        return it->index;
    }

    sound_vector.push_back(Sound(name, sound, sound_size()));
    return sound_size() - 1;
}


int Audio::push_back(Mix_Music* const music, const std::string& name)
{
    ENTER_FUNCTION(Audio::push_back);

    // overwrite any previously loaded music
    std::vector<Music>::iterator it = std::find(music_vector.begin(), music_vector.end(), name);
    if(it != music_vector.end()) {
        if(it->music) Mix_FreeMusic(it->music);
        it->music = music;
        return it->index;
    }

    music_vector.push_back(Music(name, music, music_size()));
    return music_size() - 1;
}


int Audio::load_sound(const std::string& filename)
{
    ENTER_FUNCTION(Audio::load_sound);

    // only load it if we haven't already
    std::vector<Sound>::iterator it = std::find(sound_vector.begin(), sound_vector.end(), filename);
    if(it != sound_vector.end()) {
        if(it->chunk) return it->index;

        it->chunk = Mix_LoadWAV(filename.c_str());
        if(it->chunk) return it->index;

        return -1;
    }

    Mix_Chunk* sound = Mix_LoadWAV(filename.c_str());
    if(!sound) return -1;

    sound_vector.push_back(Sound(filename, sound, sound_size()));
    return sound_size() - 1;
}


int Audio::load_music(const std::string& filename)
{
    ENTER_FUNCTION(Audio::load_music);

    // only load it if we haven't already
    std::vector<Music>::iterator it = std::find(music_vector.begin(), music_vector.end(), filename);
    if(it != music_vector.end()) {
        if(it->music) return it->index;

        it->music = Mix_LoadMUS(filename.c_str());
        if(it->music) return it->index;

        return -1;
    }

    Mix_Music* music = Mix_LoadMUS(filename.c_str());
    if(!music) return -1;

    music_vector.push_back(Music(filename, music, music_size()));
    return music_size() - 1;
}


void Audio::unload_sound(int index)
{
    ENTER_FUNCTION(Audio::unload_sound);

    if(index >= sound_size() || index < 0) return;
    //if(sound_vector[index].chunk)
        Mix_FreeChunk(sound_vector[index].chunk);
    sound_vector[index].chunk = NULL;
}


void Audio::unload_sound(const std::string& name)
{
    ENTER_FUNCTION(Audio::unload_sound);

    std::vector<Sound>::iterator it = std::find(sound_vector.begin(), sound_vector.end(), name);
    if(it != sound_vector.end()) {
        //if(it->chunk)
            Mix_FreeChunk(it->chunk);
        it->chunk = NULL;
    }
}


void Audio::unload_music(int index)
{
    ENTER_FUNCTION(Audio::unload_music);

    if(index >= music_size() || index < 0) return;
    //if(music_vector[index].music)
        Mix_FreeMusic(music_vector[index].music);
    music_vector[index].music = NULL;
}


void Audio::unload_music(const std::string& name)
{
    ENTER_FUNCTION(Audio::unload_music);

    std::vector<Music>::iterator it = std::find(music_vector.begin(), music_vector.end(), name);
    if(it != music_vector.end()) {
        //if(it->music)
            Mix_FreeMusic(it->music);
        it->music = NULL;
    }
}


void Audio::unload_sounds() throw()
{
    ENTER_FUNCTION(Audio::unload_sounds);

    for(std::vector<Sound>::iterator it = sound_vector.begin(); it != sound_vector.end(); ++it) {
        //if(it->chunk)
            Mix_FreeChunk(it->chunk);
        //it->chunk = NULL;
    }
    sound_vector.clear();
}


void Audio::unload_music() throw()
{
    ENTER_FUNCTION(Audio::unload_music);

    for(std::vector<Music>::iterator it = music_vector.begin(); it != music_vector.end(); ++it) {
        //if(it->music)
            Mix_FreeMusic(it->music);
        //it->music = NULL;
    }
    music_vector.clear();
}


void Audio::unload_all() throw()
{
    ENTER_FUNCTION(Audio::unload_all);

    unload_sounds();

    unload_music();
}


void Audio::play_sound(int index)
{
    ENTER_FUNCTION(Audio::play_sound);

    if(index >= sound_size() || index < 0) return;
    //if(sound_vector[index].chunk)
        Mix_PlayChannel(-1, sound_vector[index].chunk, 0);
}


void Audio::play_sound(const std::string& name)
{
    ENTER_FUNCTION(Audio::play_sound);

    std::vector<Sound>::iterator it = std::find(sound_vector.begin(), sound_vector.end(), name);
    if(it != sound_vector.end())
        return play_sound(it->index);
    play_sound(load_sound(name));
}


void Audio::play_music(int index)
{
    ENTER_FUNCTION(Audio::play_music);

    if(index >= music_size() || index < 0) return;
    //if(music_vector[index].music)
        Mix_PlayMusic(music_vector[index].music, -1);
}


void Audio::play_music(const std::string& name)
{
    ENTER_FUNCTION(Audio::play_music);

    std::vector<Music>::iterator it = std::find(music_vector.begin(), music_vector.end(), name);
    if(it != music_vector.end())
        return play_music(it->index);
    play_music(load_music(name));
}


int Audio::set_sound_volume(int index, int volume)
{
    ENTER_FUNCTION(Audio::set_sound_volume);

    if(index >= sound_size() || index < 0) return -1;
    //if(sound_vector[index].chunk)
        return Mix_VolumeChunk(sound_vector[index].chunk, volume);
    //return -1;
}


int Audio::get_sound_volume(int index)
{
    ENTER_FUNCTION(Audio::get_sound_volume);

    if(index >= sound_size() || index < 0) return -1;
    //if(sound_vector[index].chunk)
        return Mix_VolumeChunk(sound_vector[index].chunk, -1);
    //return -1;
}


bool Audio::start()
{
    ENTER_FUNCTION(Audio::start);

    if(Mix_OpenAudio(frequency, format, channels, buffer_size) < 0)
        return false;

    // save what we actually got (I assume bufferSize remains unchanged)
    Mix_QuerySpec(&frequency, &format, &channels);
    return true;
}


void Audio::stop() throw()
{
    ENTER_FUNCTION(Audio::stop);

    Mix_CloseAudio();
}


void Audio::stop_music()
{
    ENTER_FUNCTION(Audio::stop_music);

    Mix_HaltMusic();
}


void Audio::print_sounds(std::ostream& out)
{
    ENTER_FUNCTION(Audio::print_sounds);

    out << "I have " << sound_size() << " sound samples" << std::endl;
    for(std::vector<Sound>::const_iterator it = sound_vector.begin(); it != sound_vector.end(); ++it) {
        out << it->index << ": " << it->name << " ";

        it->chunk
            ? out << "(Length: " << it->chunk->alen << ", Volume: " << static_cast<int>(it->chunk->volume) << ")" << std::endl
            : out << "(NULL)" << std::endl;
    }
}


void Audio::print_music(std::ostream& out)
{
    ENTER_FUNCTION(Audio::print_music);

    out << "I have " << music_size() << " music samples" << std::endl;
    for(std::vector<Music>::const_iterator it = music_vector.begin(); it != music_vector.end(); ++it)
        out << it->index << ": " << it->name << " " << (it->music ? "(Info not available)" : "(NULL)") << std::endl;
}


void Audio::print_all(std::ostream& out)
{
    ENTER_FUNCTION(Audio::print_all);

    print_sounds(out);
    out << std::endl;
    print_music(out);
}


Audio::Audio() throw(Audio::AudioException)
{
    ENTER_FUNCTION(Audio::Audio);

    if(!initialize())
        throw(AudioException(SDL_GetError()));
}


Audio::~Audio() throw()
{
    ENTER_FUNCTION(Audio::~Audio);

    shutdown();
}


std::ostream& operator<<(std::ostream& lhs, const Audio& rhs)
{
    lhs << "Audio subsystem information:" << std::endl
        << "\tFrequency: " << Audio::frequency << "Hz" << std::endl
        << "\tBits: " << static_cast<int>(Audio::format & 0xff) << std::endl
        << "\tChannels: " << Audio::channels << std::endl
        << "\tBuffer: " << Audio::buffer_size << " bytes";
    return lhs;
}
