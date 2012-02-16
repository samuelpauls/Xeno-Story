/*
 * Toggle.cpp
 *
 *  Created on: 2012-01-09
 *      Author: samuel
 */

#include <GL/gl.h>
#include "Toggle.h"

using Game::UI::Toggle;
using Game::Animation;

Toggle::Toggle (bool state) throw() {
  background = new Animation ("ui/toggle_background.png");

  slider = new Animation ("ui/toggle_slider.png");
  slider_highlighted = new Animation ("ui/toggle_slider_highlighted.png");
  current_slider = slider;
  slider_rectangle.w = slider->get_width();
  slider_rectangle.h = slider->get_height();
  dragging_slider = false;

  value_changed_callback = NULL;
  value_changed_callback_data = NULL;

  set_value (state);
}

Toggle::~Toggle() throw() {
  delete background;
  delete slider;
  delete slider_highlighted;
}

void Toggle::set_value (bool value) throw() {
  this->value = value;
  if (value) {
    slider_rectangle.x = 0;
  } else {
    slider_rectangle.x = background->get_width() / 2;
  }
  if (value_changed_callback) {
    value_changed_callback (value_changed_callback_data);
  }
}

void Toggle::set_value_changed_callback (void (*function)(void *data),
                                         void *data) throw() {
  value_changed_callback = function;
  value_changed_callback_data = data;
}

void Toggle::update (unsigned int tick) throw() {
  background->update (tick);
  current_slider->update (tick);
}

void Toggle::draw() const throw() {
  // Translate to the toggle's relative location.
  glPushMatrix();
  glTranslatef (location.x, location.y, 0.0F);

  background->draw();

  // Draw the slider.
  glPushMatrix();
  glTranslatef (slider_rectangle.x, 0.0F, 0.0F);
  slider->draw();
  glPopMatrix();

  glPopMatrix();
}

Game::UI::Widget *Toggle::mouse_move (int x, int y) throw() {
  if (dragging_slider) {
    // If the mouse is in the slider's on rectangle...
    if (x - location.x < background->get_width() / 2) {
      slider_rectangle.x = 0;
      tentative_value = true;
    } else { // If the mouse is in the slider's off rectangle...
      slider_rectangle.x = background->get_width() / 2;
      tentative_value = false;
    }
  } else {
    // If the mouse is inside the slider...
    if (slider_rectangle.intersects (x - location.x, y - location.y)) {
      current_slider = slider_highlighted;
    } else {
      current_slider = slider;
    }
  }
  return this;
}

void Toggle::mouse_primary_pressed (int x, int y) throw() {
  dragging_slider = true;

  // If the user doesn't drag the slider, toggle the value to it's opposite upon
  // the primary mouse button's release.
  tentative_value = !value;
}

void Toggle::mouse_primary_released (int x, int y) throw() {
  dragging_slider = false;
  if (value != tentative_value) {
    set_value (tentative_value);
  }
}

void Toggle::mouse_exit() throw() {
  current_slider = slider;
}
