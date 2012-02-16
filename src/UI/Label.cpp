/*
 * Label.cpp
 *
 *  Created on: 2011-12-29
 *      Author: samuel
 */

#include "../Clipper.h"
#include "Label.h"

using Game::UI::Label;

const Game::Font Label::FONT (10);

Label::Label (const std::string &text) throw() {
  this->text = text;
}

Label::~Label() throw() {}

void Label::draw() const throw() {
  // Translate to the label's relative location.
  glPushMatrix();
  int x = location.x;
  int y = location.y - (FONT.get_line_height() - location.h) / 2;
  glTranslatef (x, y, 0.0F);

  glColor3ub (64, 40, 196); // dark purple
  Clipper::push (get_position_in_window());
  FONT.draw (text);
  Clipper::pop();

  glPopMatrix();
}
