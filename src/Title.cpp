/*
 * Title.cpp
 *
 *  Created on: 2011-10-13
 *      Author: samuel
 */

#include <GL/gl.h>
#include "Window.h"
#include "Title.h"

using Game::Title;

Game::Font Title::text (30);

Game::Audio Title::sound ("title_flash.ogg");

Title::Title (const std::string &string) throw() {
  this->string = string;

  start_tick = 0;
  done = false;
}

Title::~Title() throw() {}

void Title::update (unsigned int tick) throw() {
  if (start_tick == 0) {
    start_tick = tick;
    sound.play();
  } else {
    static const unsigned int ACQUISITION_DURATION = 3000; // in ms
    if (tick > start_tick + ACQUISITION_DURATION) {
      done = true;
    }
  }
}

void Title::draw() const throw() {
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef ((Window::get_width() - text.get_width (string)) / 2.0F,
                Window::get_height() * 0.4F,
                0.0F);

  glColor3f (1.0F, 1.0F, 1.0F); // white

  text.draw (string);
}
