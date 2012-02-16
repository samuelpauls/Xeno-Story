/*
 * ScrollableContainer.h
 *
 *  Created on: 2012-01-03
 *      Author: samuel
 */

#ifndef SCROLLABLECONTAINER_H
#define SCROLLABLECONTAINER_H

#include "../Rectangle.h"
#include "Widget.h"
#include "Layout.h"

namespace Game {
  namespace UI {
    // Scrolls through a child widget, such that only a portion of the child is
    // visible.
    class ScrollableContainer: public Layout {
    public:
      ScrollableContainer() throw();
      virtual ~ScrollableContainer() throw();

      virtual void set_position (const Rectangle &location) throw();

      // Consume a child widget and delete it upon deletion of this frame.
      // Can't consume more than one widget at a time.
      void consume (Widget *widget) throw();

      // Forget about the child widget and no longer delete it upon container
      // destruction.
      void unconsume() throw();

      // Called every frame, even if no user events occur.
      // tick is in ms
      virtual void update (unsigned int tick) throw();

      virtual void draw() const throw();
    };
  } // namespace UI
} // namespace Game

#endif // SCROLLABLECONTAINER_H
