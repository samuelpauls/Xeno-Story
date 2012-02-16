/*
 * Audio.cpp
 *
 *  Created on: 2011-09-18
 *      Author: samuel
 */

#include "UserSettings.h"
#include "Audio.h"

using Game::Audio;

Audio::Audio (const std::string &filename, bool loop) throw() {
  this->filename = filename;
  buffer.LoadFromFile (filename);
  sound.SetBuffer (buffer);
  sound.SetLoop (loop);
}

Audio::~Audio() {}

void Audio::play() throw() {
  if (UserSettings::is_audio_enabled()) {
    sound.Play();
  }
}
