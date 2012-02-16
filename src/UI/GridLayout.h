/*
 * GridLayout.h
 *
 *  Created on: 2012-01-11
 *      Author: samuel
 */

#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H

#include "../Rectangle.h"
#include "../Layout2D.h"
#include "Widget.h"
#include "Layout.h"

namespace Game {
  namespace UI {
    // Lays widgets in a grid.
    class GridLayout: public Layout {
    private:
      int item_width, item_height; // in pixels
      int item_padding; // in pixels
      int maximum_width, maximum_height; // in pixels
      Layout2D<Widget*> *widget_grid;

      void calculate_widget_size() throw();

      void position_widget (int column, int row, Widget *widget) throw();

    public:
      // padding, maximum width/height in pixels
      static const int UNLIMITED = 0;
      GridLayout (int columns, int rows,
                  int item_padding = 0,
                  int maximum_width = UNLIMITED, int maximum_height = UNLIMITED)
                 throw();
      virtual ~GridLayout() throw();

      virtual void set_position (const Rectangle &location) throw();

      // column and row index from the top-left
      void consume (int column, int row, Widget *widget) throw();

      void unconsume (int column, int row) throw();
    };
  } // namespace UI
} // namespace Game

#endif // GRIDLAYOUT_H
