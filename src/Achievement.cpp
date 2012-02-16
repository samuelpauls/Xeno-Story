/*
 * Achievement.cpp
 *
 *  Created on: 2011-10-13
 *      Author: samuel
 */

#include <fstream>
#include <GL/gl.h>
#include "Rectangle.h"
#include "Window.h"
#include "Achievement.h"

using Game::Achievement;

Game::Audio Achievement::get_sound ("achievements/get.ogg");
Game::Animation *Achievement::background = NULL;
Game::Font Achievement::text (10);

Achievement::Achievement (const std::string &type) throw() {
  // Set the achievement background once after initialising OpenGL.
  if (!background) {
    background = new Animation ("achievements/background.png");
  }

  icon = new Animation ("achievements/" + type + "/icon.png");

  std::string data_filename = "achievements/" + type + "/general.dat";
  std::ifstream f (data_filename.c_str());

  std::getline (f, description);

  f.close();

  start_tick = 0;
  done = false;
}

Achievement::~Achievement() throw() {
  delete icon;
}

void Achievement::update (unsigned int tick) throw() {
  if (start_tick == 0) {
    start_tick = tick;
    get_sound.play();
  } else {
    static const unsigned int DISPLAY_DURATION = 3000; // in ms
    if (tick > start_tick + DISPLAY_DURATION) {
      done = true;
    }
  }

  background->update (tick);
  icon->update (tick);
}

void Achievement::draw() const throw() {
  glPushMatrix(); // save the position before drawing
  glLoadIdentity();

  // Translate to the achievement.
  static const GLfloat PADDING = 10.0F;
  GLfloat x = Window::get_width() - PADDING - background->get_width();
  GLfloat y = Window::get_height() - PADDING - background->get_height();
  glTranslatef (x, y, 0.0F);

  // Draw the background.
  background->draw();

  // Draw the icon.
  glPushMatrix();
  glTranslatef (4.0F, 4.0F, 0.0F);
  icon->draw();
  glPopMatrix();

  // Draw the description.
  glPushMatrix();
  glTranslatef (34.0F, 15.0F, 0.0F);
  glColor3ub (156, 120, 252); // light purple
  text.draw (description);
  glPopMatrix();

  glPopMatrix(); // return to the position from before drawing
}
