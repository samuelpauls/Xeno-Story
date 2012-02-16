/*
 * UserSettings.h
 *
 *  Created on: 2011-12-28
 *      Author: samuel
 */

#ifndef USERSETTINGS_H
#define USERSETTINGS_H

namespace Game {
  // Loads and saves user settings such as if audio is turned on.
  class UserSettings {
  private:
    // We store lighting as a setting, as opposed to automatically determining
    // if it's enabled based on performance.  Suppose performance is good so we
    // enable lighting.  Lighting causes performance to drop, perhaps to the
    // point that we automatically disable lighting.  Performance improves and
    // the cycle repeats itself.  To get out of this enable/disable cycle, we
    // make lighting a user setting.
    static bool lighting_enabled;

    static bool audio_enabled;

    // This class can't be instantiated.  If multiple instances were allowed,
    // changes to the settings in one instance wouldn't be immediately available
    // to another non-refreshed instance.
    UserSettings() throw();
    ~UserSettings() throw();

    static void save() throw();

  public:
    // Load user settings from disk.
    static void load() throw();

    static bool is_lighting_enabled() throw() { return lighting_enabled; }
    static void set_lighting (bool enabled);

    static bool is_audio_enabled() throw() { return audio_enabled; }
    static void set_audio (bool enabled);
  };
} // namespace Game

#endif // USERSETTINGS_H
