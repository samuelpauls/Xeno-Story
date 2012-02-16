/*
 * FocusableWidget.h
 *
 *  Created on: 2012-01-12
 *      Author: samuel
 */

#ifndef FOCUSABLEWIDGET_H
#define FOCUSABLEWIDGET_H

#include "Widget.h"

namespace Game {
  namespace UI {
    class FocusableWidget: public Widget {
    protected:
      bool focus;

    public:
      FocusableWidget() throw();
      virtual ~FocusableWidget() throw();

      virtual void set_focus (bool focus) throw() {
        this->focus = focus;
      }
    };
  } // namespace UI
} // namespace Game

#endif // FOCUSABLEWIDGET_H
