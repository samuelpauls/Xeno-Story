/*
 * UserSettingsPanel.h
 *
 *  Created on: 2012-01-09
 *      Author: samuel
 */

#ifndef USERSETTINGSPANEL_H
#define USERSETTINGSPANEL_H

#include "UI/Manager.h"
#include "UI/PaletteLayout.h"
#include "UI/Toggle.h"

namespace Game {
  // Displays general game settings and accepts changes from the user.
  class UserSettingsPanel {
  private:
    UI::Manager ui_manager;
    UI::PaletteLayout *grid;
    UI::Toggle *audio_toggle;
    UI::Toggle *lighting_toggle;

    // static because this function is used as a callback by a widget.
    static void audio_toggled (void *user_settings_widget) throw();

    // static because this function is used as a callback by a widget.
    static void lighting_toggled (void *user_settings_widget) throw();

  public:
    UserSettingsPanel() throw();
    virtual ~UserSettingsPanel() throw();

    // ticks in ms
    void update (unsigned int tick) throw() { ui_manager.update (tick); }

    void draw() const throw() { ui_manager.draw(); }
  }; // class UserSettingsPanel
} // namespace Game

#endif // USERSETTINGSPANEL_H
