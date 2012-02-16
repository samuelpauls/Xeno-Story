/*
 * Camera.cpp
 *
 *  Created on: 2011-08-16
 *      Author: samuel
 */

#include <GL/gl.h>
#include "Window.h"
#include "Camera.h"

using Game::Camera;

Camera::Camera (int viewport_width, int viewport_height) throw() {
  viewport.x = 0;
  viewport.y = 0;
  viewport.w = viewport_width;
  viewport.h = viewport_height;

  follow_sprite = NULL;
}

Camera::~Camera() throw() {
}

void Camera::move (int x, int y) throw() {
  viewport.x += x;
  viewport.y += y;
}

void Camera::update() throw() {
  if (follow_sprite) {
    Rectangle sprite_rect = follow_sprite->get_position();

    static const int PADDING = 200;

    Rectangle focus_rect (viewport);
    focus_rect.x += PADDING;
    focus_rect.y += PADDING;
    focus_rect.w -= PADDING * 2;
    focus_rect.h -= PADDING * 2;

    // Ensure the sprite is always within the viewport's focus.
    if (sprite_rect.x < focus_rect.x) {
      viewport.x = sprite_rect.x - PADDING;
    } else if (sprite_rect.get_right() > focus_rect.get_right()) {
      viewport.x = sprite_rect.get_right() - viewport.w + PADDING;
    }
    if (sprite_rect.get_top() > focus_rect.get_top()) {
      viewport.y = sprite_rect.get_top() + PADDING - viewport.h;
    } else if (sprite_rect.y < focus_rect.y) {
      viewport.y = sprite_rect.y - PADDING;
    }
  }
}

void Camera::translate_view() throw() {
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef (-viewport.x, -viewport.y, 0.0F);
}
