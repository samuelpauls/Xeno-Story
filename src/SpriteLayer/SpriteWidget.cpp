/*
 * SpriteWidget.cpp
 *
 *  Created on: 2011-12-30
 *      Author: samuel
 */

#include <GL/gl.h>
#include "../Clipper.h"
#include "SpriteWidget.h"

using Game::SpriteLayer::SpriteWidget;

SpriteWidget::SpriteWidget (const std::string &subtype) throw() {
  sprite = Sprite::create (subtype);
}

SpriteWidget::~SpriteWidget() throw() {
  delete sprite;
}

void SpriteWidget::draw() const throw() {
  glPushMatrix();
  glTranslatef (location.x, location.y, 0.0F);

  // Clip the part of the sprite that may be drawn outside of this widget.
  Clipper::push (get_position_in_window());

  glPushMatrix();
  GLfloat x =  -(sprite->get_position().w - location.w) / 2.0F;
  GLfloat y =  -(sprite->get_position().h - location.h) / 2.0F;
  glTranslatef (x, y, 0.0F);
  sprite->draw (NULL);
  glPopMatrix();

  Clipper::pop();

  // Draw a box around the sprite if it's in focus.
  if (highlighted || selected) {
    if (selected) {
      glColor3f (1.0F, 1.0F, 1.0F); // white
    } else if (highlighted) {
      glColor3ub (156, 120, 152); // light purple
    }
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    glRecti (1, 0, location.w, location.h - 1);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }

  glPopMatrix();
}
