/**
\file Audio.h
\author Shane Lillie
\brief Audio subsystem module header.

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


#if !defined AUDIO_H
#define AUDIO_H


#include "shared.h"


/**
\class Audio
\brief Audio subsystem class.

The interface is pretty straight-forward, just initialize the subsystem,
load up a sound or music file, and away you go.

An automatic object can be created to initialize and shutdown the subsystem on the fly.

Sounds and musics are stored with both an index and a name for lookup.

\note Volumes range from 0 to 128, per the SDL_Mixer documentation.

\todo Try to fully functionalize the SDL_Mixer features (fading, etc).
\todo The class variables should be loaded from a config file.
\todo Make the internal classes pimpl based or hidden by some other means.
*/
class Audio
{
private:
    /**
    \struct Sound
    \brief Internal representation of a sound file.
    */
    struct Sound
    {
        std::string name;
        Mix_Chunk* chunk;
        int index;

        Sound(const std::string& n, Mix_Chunk* c, int i)
            : name(n), chunk(c), index(i)
        {
        }

        friend bool operator==(const Sound& lhs, const std::string& rhs)
        {
            return lhs.name == rhs;
        }
    };

    /**
    \struct Music
    \brief Internal representation of a music file.
    */
    struct Music
    {
        std::string name;
        Mix_Music* music;
        int index;

        Music(const std::string& n, Mix_Music* m, int i)
            : name(n), music(m), index(i)
        {
        }

        friend bool operator==(const Music& lhs, const std::string& rhs)
        {
            return lhs.name == rhs;
        }
    };

public:
    /**
    \brief Initializes the subsystem.
    @retval true Initialization succeeded.
    @retval false Initialization failed. Use SDL_GetError() for details.
    */
    static bool initialize();

    /**
    \brief Shuts the subsystem down.
    */
    static void shutdown() throw();

    /**
    \brief Adds a loaded sound with the given name to the subsystem.
    @param sound The sound chunk to add.
    @param name The name to give the sound.
    @return The index of the file.
    @note This frees the first previously loaded sound file with the given name.
    */
    static int push_back(Mix_Chunk* const sound, const std::string& name);

    /**
    \brief Adds a loaded music chunk with the given name to the subsystem.
    @param music The music chunk to add.
    @param name The name to give the music.
    @return The index of the file.
    @note This frees the first previously loaded music file with the given name.
    */
    static int push_back(Mix_Music* const music, const std::string& name);

    /**
    \brief Loads a sound file and adds it to the subsystem.
    @param filename The filename of the file to load.
    @return The index of the sound.
    @retval -1 An error occured. Use SDL_GetError for details.
    @note This only loads the file if it wasn't loaded successfully already.
    */
    static int load_sound(const std::string& filename);

    /**
    \brief Loads a music file and adds it to the subsystem.
    @param filename the filename of the file to load.
    @return The index of the music.
    @retval -1 An error occured. Use SDL_GetError() for details.
    @note This only loads the file if it wasn't loaded successfully already.
    */
    static int load_music(const std::string& filename);

    /**
    \brief Unloads a sound.
    @param index The index of the sound to unload.
    */
    static void unload_sound(int index);

    /**
    \brief Unloads the first sound with the given name.
    @param name The name of the sound to unload.
    */
    static void unload_sound(const std::string& name);

    /**
    \brief Unloads a music.
    @param index The index of the music to unload.
    */
    static void unload_music(int index);

    /**
    \brief Unloads the first music with the given name.
    @param name The name of the music to unload.
    */
    static void unload_music(const std::string& name);

    /**
    \brief Unloads all sounds.
    */
    static void unload_sounds() throw();

    /**
    \brief Unloads all musics.
    */
    static void unload_music() throw();

    /**
    \brief Unloads all sounds and musics.
    */
    static void unload_all() throw();

    /**
    \brief Plays a sound.
    @param index The index of the sound to play.
    */
    static void play_sound(int index);

    /**
    \brief Plays the first sound with the given name.
    @param name The name of the sound to play.
    @note This will try to load the sound if it isn't already loaded by assuming name is a filename.
    */
    static void play_sound(const std::string& name);

    /**
    \brief Plays a music.
    @param index The index of the sound to play.
    */
    static void play_music(int index);

    /**
    \brief Plays the first music with the given name.
    @param name The name of the music to play.
    @note This will try to load the music if it isn't already loaded by assuming name is a filename.
    */
    static void play_music(const std::string& name);

    /**
    \brief Sets the volume of a sound.
    @param index The index of the sound.
    @param volume The new volume amount.
    @return Mix_VolumeChunk().
    @retval -1 The index is out of bounds.
    */
    static int set_sound_volume(int index, int volume);

    /**
    \brief Gets the volume of a sound.
    @param index The index of the sound.
    @return Mix_VolumeChunk().
    @retval -1 The index is out of bounds.
    */
    static int get_sound_volume(int index);

    /**
    \brief Starts the subsystem.
    @retval true Success.
    @retval false Failure. Use SDL_GetError() for details.
    */
    static bool start();

    /**
    \brief Stops all audio.
    */
    static void stop() throw();

    /**
    \brief Stops all playing music.
    */
    static void stop_music();

    /**
    \brief Prints the info for all the sounds.
    @param out The output stream to print to.
    */
    static void print_sounds(std::ostream& out);

    /**
    \brief Prints the info for all the musics.
    @param out The output stream to print to.
    */
    static void print_music(std::ostream& out);

    /**
    \brief Prints the info for all the sounds and music.s
    @param out The output stream to print to.
    */
    static void print_all(std::ostream& out);

public:
    /**
    \brief Sets the overall volume.
    @return Mix_Volume().
    */
    static int set_volume(int volume)
    {
        return Mix_Volume(-1, volume);
    }

    /**
    @return The overall volume.
    */
    static int get_volume()
    {
        return Mix_Volume(-1, -1);
    }

    /**
    \brief Sets the music volume.
    @return Mix_VolumeMusic().
    */
    static int set_music_volume(int volume)
    {
        return Mix_VolumeMusic(volume);
    }

    /**
    @return The music volume.
    */
    static int get_music_volume()
    {
        return Mix_VolumeMusic(-1);
    }

private:
    static int sound_size() { return static_cast<int>(sound_vector.size()); }
    static int music_size() { return static_cast<int>(music_vector.size()); }

private:
    static std::vector<Sound> sound_vector;
    static std::vector<Music> music_vector;

    static int buffer_size, frequency, channels;
    static Uint16 format;

public:
    /**
    \class AudioException
    \brief Generic subsystem exception.
    */
    class AudioException : public std::exception
    {
    public:
        AudioException(const std::string& what) throw() : _what(what) { }
        virtual ~AudioException() throw() { }
        virtual const char* what() const throw() { return _what.c_str(); }
    private:
        std::string _what;
    };

public:
    /**
    \brief Initializes the subsystem.
    \throw AudioException Contains the error that occured on initialization.
    */
    Audio() throw(AudioException);

    /**
    \brief Shuts down the subsystem.
    */
    ~Audio() throw();

public:
    /**
    \brief Prints the subsystem information.
    @param lhs The output stream to print to.
    @param rhs Not used.
    */
    friend std::ostream& operator<<(std::ostream& lhs, const Audio& rhs);
};


#endif
