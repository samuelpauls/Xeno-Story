/*
 * MenuTextItem.cpp
 *
 *  Created on: 2012-01-01
 *      Author: samuel
 */

#include "MenuTextItem.h"

using Game::UI::MenuTextItem;

MenuTextItem::MenuTextItem (
    const std::string &text,
    void (*select_callback) (void *select_callback_data) throw(),
    Theme theme) throw() {
  set_text (text);
  if (theme == NORMAL) {
    font = new Font (10);
  } else if (theme == LARGE) {
    font = new Font (20);
  }

  set_select_callback (select_callback);
}

MenuTextItem::~MenuTextItem() throw() {
  delete font;
}

void MenuTextItem::draw() const throw() {
  // Translate to the widget's relative location.
  glPushMatrix();
  int x = location.x - (font->get_width (get_text()) - location.w) / 2;
  int y = location.y - (font->get_line_height() - location.h) / 2;
  glTranslatef (x, y, 0.0F);

  if (selected) {
    glColor3f (1.0F, 1.0F, 1.0F); // white
  } else if (highlighted) {
    glColor3ub (156, 120, 152); // light purple
  } else {
    glColor3ub (156, 120, 252); // purple
  }
  font->draw (get_text());

  glPopMatrix();
}
