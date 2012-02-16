/*
 * UserSettings.cpp
 *
 *  Created on: 2011-12-28
 *      Author: samuel
 */

#include <fstream>
#include "UserSettings.h"

using Game::UserSettings;

bool UserSettings::lighting_enabled = false;
bool UserSettings::audio_enabled = false;

UserSettings::UserSettings() throw() {}

UserSettings::~UserSettings() throw() {}

void UserSettings::save() throw() {
  std::ofstream f ("user_settings.dat");
  f << lighting_enabled << std::endl;
  f << audio_enabled;
  f.close();
}

void UserSettings::load() throw() {
  std::ifstream f ("user_settings.dat");
  f >> lighting_enabled;
  f >> audio_enabled;
  f.close();
}

void UserSettings::set_lighting (bool enabled) {
  lighting_enabled = enabled;
  save();
}

void UserSettings::set_audio (bool enabled) {
  audio_enabled = enabled;
  save();
}
