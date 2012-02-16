/*
 * MessageBox.cpp
 *
 *  Created on: 2011-08-24
 *      Author: samuel
 */

#include "MessageBox.h"

using Game::MessageBox;

Game::Font MessageBox::text (10);

MessageBox::MessageBox (const std::string &name,
                        const std::string &message) throw() {
  this->name = name;
  this->message = message;

  background = new Animation ("message_box/background.png");
  portrait = new Animation ("message_box/" + name + ".png");
  proceed = new Animation ("message_box/proceed.png");
}

MessageBox::~MessageBox() throw() {
  delete background;
  delete portrait;
  delete proceed;
}

void MessageBox::update (unsigned int tick) throw() {
  background->update (tick);
  portrait->update (tick);
  proceed->update (tick);
}

void MessageBox::draw() const throw() {
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  // Translate to the message box.
  static const int BACKGROUND_WIDTH = 273;
  static const GLfloat X = (Window::get_width() - BACKGROUND_WIDTH) / 2;
  static const int PADDING_FROM_SCREEN_EDGE = 10;
  glTranslatef (X, PADDING_FROM_SCREEN_EDGE, 0.0F);

  background->draw();

  // Draw the portrait.
  glPushMatrix();
  glTranslatef (5.0F, 5.0F, 0.0F);
  portrait->draw();
  glPopMatrix();

  // Draw the proceed button.
  glPushMatrix();
  glTranslatef (235.0F, 65.0F, 0.0F);
  proceed->draw();
  glPopMatrix();

  glColor3ub (64, 40, 196); // dark purple

  // Draw the speaker's name.
  glPushMatrix();
  glTranslatef (5.0F, 79.0F, 0.0F);
  text.draw (name);
  glPopMatrix();

  // Draw the message.
  glPushMatrix();
  glTranslatef (75.0F, 60.0F, 0.0F);
  text.draw (message);
  glPopMatrix();
}
