/*
 * PaletteLayout.h
 *
 *  Created on: 2012-01-03
 *      Author: samuel
 */

#ifndef PALETTELAYOUT_H
#define PALETTELAYOUT_H

#include <vector>
#include "Widget.h"
#include "Layout.h"

namespace Game {
  namespace UI {
    // A layout of ordered widgets that conform to the layout's size.
    class PaletteLayout: public Layout {
    private:
      int item_width, item_height; // in pixels

      Rectangle calculate_widget_position (int index) const throw();

    protected:
      int item_padding; // space in pixels between items and between items and this
                   // grid's border

    public:
      // padding: space in pixels between items
      PaletteLayout (int item_width, int item_height,
                     int item_padding = 0) throw();
      virtual ~PaletteLayout() throw();

      virtual void set_position (const Rectangle &location) throw();

      // Consume a widget that is automatically deleted at the time this layout
      // is deleted.
      void consume (Widget *widget) throw();

      // Unconsume a child widget and no longer take responsibility for deleting
      // it upon destruction of this layout.
      void unconsume (int index) throw();

      int get_item_count() const throw() { return child_widgets.size(); }

      Widget *get_widget (int index) const throw() {
        return child_widgets[index];
      }
    }; // class PaletteLayout
  } // namespace UI
} // namespace Game

#endif // PALETTELAYOUT_H
