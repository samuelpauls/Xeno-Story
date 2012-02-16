/*
 * Clipper.h
 *
 *  Created on: 2012-01-08
 *      Author: samuel
 */

#ifndef CLIPPER_H
#define CLIPPER_H

#include "Rectangle.h"
#include <stack>

namespace Game {
  // Provides a clipping stack.  Needed because OpenGL doesn't support a stack
  // of intersecting clipping rectangles.
  class Clipper {
  private:
    static std::stack<Rectangle> clip_stack;

    // Prevent instantiation.
    Clipper() throw();
    virtual ~Clipper() throw();

  public:
    // Push a clipping rectangle onto the stack, thereby intersecting the
    // current overall clipping rectangle.
    static void push (const Rectangle &clip_rectangle) throw();

    // Pop the most recent clipping rectangle off the stack.
    static void pop() throw();
  };
} // namespace Game

#endif // CLIPPER_H
