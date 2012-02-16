/*
 * Clipper.cpp
 *
 *  Created on: 2012-01-08
 *      Author: samuel
 */

#include <cassert>
#include <GL/gl.h>
#include "Clipper.h"

using Game::Clipper;

std::stack<Game::Rectangle> Clipper::clip_stack;

Clipper::Clipper() throw() {}

Clipper::~Clipper() throw() {}

void Clipper::push (const Rectangle &clip_rectangle) throw() {
  Rectangle merged_rect;
  if (clip_stack.empty()) {
    glEnable (GL_SCISSOR_TEST); // disabled by default
    merged_rect = clip_rectangle;
  } else {
    merged_rect = clip_stack.top().intersection (clip_rectangle);
  }
  clip_stack.push (merged_rect);
  glScissor (merged_rect.x, merged_rect.y, merged_rect.w, merged_rect.h);
}

void Clipper::pop() throw() {
  assert (!clip_stack.empty());
  clip_stack.pop();
  if (clip_stack.empty()) {
    glDisable (GL_SCISSOR_TEST);
  } else {
    Rectangle rect = clip_stack.top();
    glScissor (rect.x, rect.y, rect.w, rect.h);
  }
}
