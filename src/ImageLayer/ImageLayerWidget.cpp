/*
 * ImageLayerWidget.cpp
 *
 *  Created on: 2012-01-01
 *      Author: samuel
 */

#include <GL/gl.h>
#include "ImageLayerWidget.h"

using Game::ImageLayer::ImageLayerWidget;

ImageLayerWidget::ImageLayerWidget (Animation *animation,
                                    const std::string &filename) throw() {
  this->filename = filename;
  this->animation = animation;
}

ImageLayerWidget::~ImageLayerWidget() throw() {
  delete animation;
}

void ImageLayerWidget::draw() const throw() {
  glPushMatrix();
  glTranslatef (location.x, location.y, 0.0F);

  // Draw the image thumbnail.
  glPushMatrix();
  GLfloat scale = (GLfloat)location.h / animation->get_height();
  glScalef (scale, scale, 1.0F);
  animation->draw();
  glPopMatrix();

  // Draw a box around the selected image in the palette.
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
