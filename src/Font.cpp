/*
 * Font.cpp
 *
 *  Created on: 2011-11-03
 *      Author: samuel
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include "Window.h"
#include "Font.h"

using Game::Font;

Font::Font (int size) throw() {
  drawable_string.SetSize (size);
}

Font::~Font() throw() {}

int Font::get_width (const std::string &text) const throw() {
  drawable_string.SetText (text);
  return drawable_string.GetRect().GetWidth();
}

int Font::get_height (const std::string &text) const throw() {
  drawable_string.SetText (text);
  return drawable_string.GetRect().GetHeight();
}

void Font::draw (const std::string &text) const throw() {
  // Save the current matrix mode and texture environment.
  glPushAttrib (GL_TRANSFORM_BIT | GL_TEXTURE_BIT);

  drawable_string.SetText (text);

  // Set the text color to that of OpenGL.
  GLfloat gl_color[4];
  glGetFloatv (GL_CURRENT_COLOR, gl_color);
  sf::Color sfml_color;
  sfml_color.r = gl_color[0] * 255.0F;
  sfml_color.g = gl_color[1] * 255.0F;
  sfml_color.b = gl_color[2] * 255.0F;
  sfml_color.a = gl_color[3] * 255.0F;
  drawable_string.SetColor (sfml_color); // SFML's default is white
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  GLdouble win_x, win_y, win_z;

  GLdouble modelview[16], projection[16];
  GLint viewport[4];
  glGetDoublev (GL_MODELVIEW_MATRIX, modelview);
  glGetDoublev (GL_PROJECTION_MATRIX, projection);
  glGetIntegerv (GL_VIEWPORT, viewport);
  gluProject (0.0, 0.0, 0.0, // object
              modelview,
              projection,
              viewport,
              &win_x, &win_y, &win_z);

  // sf::String::SetPosition expects the top-left coordinate of the text with
  // the window's origin at the top-left.  We provide the bottom-left of the
  // first line of text.
  drawable_string.SetPosition (
      win_x, Window::get_height() - (win_y + get_line_height()));

  glPushMatrix(); // save the current modelview matrix
  glMatrixMode (GL_PROJECTION);
  glPushMatrix();
  Window::app->Draw (drawable_string);
  glMatrixMode (GL_PROJECTION);
  glPopMatrix();
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib(); // restore the previous matrix mode and texture environment
}
