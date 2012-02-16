/*
 * Editor.cpp
 *
 *  Created on: 2011-08-19
 *      Author: samuel
 */

#include "Editor.h"

using Game::Controllers::Editor;

Editor::Editor() {
  toggle_pressed = false;

  scroll_left_pressed = false;
  scroll_right_pressed = false;
  scroll_up_pressed = false;
  scroll_down_pressed = false;

  next_layer = false;
  previous_layer = false;

  next_brush = false;
  previous_brush = false;
}

Editor::~Editor() {}
