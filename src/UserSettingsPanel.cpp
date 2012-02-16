/*
 * UserSettingsPanel.cpp
 *
 *  Created on: 2012-01-09
 *      Author: samuel
 */

#include "UserSettings.h"
#include "UI/Label.h"
#include "UserSettingsPanel.h"

using Game::UserSettingsPanel;
using Game::UI::PaletteLayout;
using Game::UI::Label;
using Game::UI::Toggle;

void UserSettingsPanel::audio_toggled (void *user_settings_widget) throw() {
  UserSettingsPanel *casted = (UserSettingsPanel*)user_settings_widget;
  UserSettings::set_audio (casted->audio_toggle->get_value());
}

void UserSettingsPanel::lighting_toggled (void *user_settings_widget) throw() {
  UserSettingsPanel *casted = (UserSettingsPanel*)user_settings_widget;
  UserSettings::set_lighting (casted->lighting_toggle->get_value());
}

UserSettingsPanel::UserSettingsPanel() throw() {
  static const int ITEM_WIDTH = 100; // pixels
  static const int ITEM_HEIGHT = 15; // pixels
  grid = new PaletteLayout (ITEM_WIDTH, ITEM_HEIGHT);
  ui_manager.consume (grid);

  // TODO: Use real menu rect from main.cpp
  Rectangle menu_rect;
  menu_rect.w = ITEM_WIDTH * 2;
  menu_rect.h = 300;
  menu_rect.x = 400;
  menu_rect.y = (Window::get_height() - menu_rect.h) / 2;
  grid->set_position (menu_rect);

  grid->consume (new Label ("Audio:"));
  audio_toggle = new Toggle (UserSettings::is_audio_enabled());
  audio_toggle->set_value_changed_callback (audio_toggled, this);
  grid->consume (audio_toggle);

  grid->consume (new Label ("Lighting:"));
  lighting_toggle = new Toggle (UserSettings::is_lighting_enabled());
  lighting_toggle->set_value_changed_callback (lighting_toggled, this);
  grid->consume (lighting_toggle);
}

UserSettingsPanel::~UserSettingsPanel() throw() {
  ui_manager.unconsume (grid);
  delete grid;
}
