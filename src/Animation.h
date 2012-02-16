/*
 * Animation.h
 *
 *  Created on: 2011-08-10
 *      Author: samuel
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <GL/gl.h>
#include "Window.h"
#include "Rectangle.h"

namespace Game {
  class Window;

  // A sprite that interacts with the game world.
  class Animation {
  public:
    enum Mode {
      FORWARD,
      BACKWARD,
    };

  private:
    struct Common {
      int frame_count;
      int frame_duration; // in ms
      Mode mode;

      int animation_width, animation_height; // in pixels
      GLuint texture;

      int reference_count;
    } *common;

    int frame_index;

    bool flip_x;

    Rectangle source_rectangle;
    int offset_x, offset_y; // offset in pixels of the drawing location

    unsigned int start_tick; // in ms
    unsigned int elapsed_ticks; // in ms

  public:
    // Load an animation from a file.  An optional data file may accompany it.
    Animation (const std::string &graphic_filename) throw();

    Animation (const Animation &animation) throw();

    virtual ~Animation() throw();

    // TODO: Maybe move to the avatar drawing method.
    void flip_horizontally() throw() { flip_x = !flip_x; }

    // Update the animation's frame.
    // tick is ms since animation start
    void update (unsigned int tick) throw();

    // Rewind to the first frame.
    // tick is ms since animation start
    void rewind (unsigned int tick) throw();

    // Draw the animation with the OpenGL origin being the bottom-left.
    void draw() throw();

    bool is_done() const throw();

    // Returns the number of times this animation has played since rewound.
    float get_times_played() const throw();

    // Return the width of a frame in pixels.
    int get_width() const throw() { return source_rectangle.w; }

    // Return the height of a frame in pixels.
    int get_height() const throw() { return source_rectangle.h; }

    // TODO: Maybe remove offsets from the animation.
    void set_draw_offset_x (int offset) throw() { offset_x = offset; }
    void set_draw_offset_y (int offset) throw() { offset_y = offset; }
    int get_draw_offset_x() const throw() { return offset_x; }
    int get_draw_offset_y() const throw() { return offset_y; }
  }; // class Animation
} // namespace Game

#endif // ANIMATION_H
