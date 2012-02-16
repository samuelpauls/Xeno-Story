/*
 * Audio.h
 *
 *  Created on: 2011-09-18
 *      Author: samuel
 */

#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <SFML/Audio.hpp>

namespace Game {
  // Provides sound capabilities in a library independent wrapper.
  class Audio {
  private:
    std::string filename;
    sf::SoundBuffer buffer;
    sf::Sound sound;

  public:
    // loop causes the sound to repeat until stop() is called.
    Audio (const std::string &filename, bool loop = false) throw();
    virtual ~Audio();

    std::string get_filename() const throw() { return filename; }

    void play() throw();
    void stop() throw() { sound.Stop(); }
  };
}

#endif // AUDIO_H
