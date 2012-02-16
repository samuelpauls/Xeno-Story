/*
 * Frame.h
 *
 *  Created on: 2011-12-21
 *      Author: samuel
 */

#ifndef FRAME_H
#define FRAME_H

#include <string>
#include "../NineSlice.h"
#include "../Font.h"
#include "../Rectangle.h"
#include "Widget.h"
#include "Layout.h"

namespace Game {
  namespace UI {
    // A movable frame with a title that holds widgets.
    class Frame: public Layout {
    private:
      NineSlice background;

      Rectangle title_rectangle; // in pixels from the bottom-left of frame
      std::string title;
      static Font title_font;

      bool dragging;
      int drag_x, drag_y; // in pixels from the bottom-left of frame

      bool resizing_left, resizing_right, resizing_bottom;

    public:
      static const int PADDING = 5; // in pixels
      static const int TOP_PADDING = 13; // in pixels

      Frame (const std::string &title) throw();
      virtual ~Frame() throw();

      virtual void set_position (const Rectangle &location) throw();

      // Returns the position that a child widget occupies within this frame.
      Rectangle get_child_position();

      // Consume a child widget and delete it upon deletion of this frame.
      // Can't consume more than one widget at a time.
      void consume (Widget *widget) throw();

      // Forget about the child widget and no longer delete it upon frame
      // destruction.
      void unconsume() throw();

      virtual void draw() const throw();

      virtual Widget *mouse_move (int x, int y) throw();
      virtual void mouse_primary_pressed (int x, int y) throw();
      virtual void mouse_primary_released (int x, int y) throw();
    };
  } // namespace UI
} // namespace Game

#endif // FRAME_H
