/*
 * MenuTextItem.h
 *
 *  Created on: 2012-01-01
 *      Author: samuel
 */

#ifndef MENUTEXTITEM_H
#define MENUTEXTITEM_H

#include <string>
#include "../Font.h"
#include "Widget.h"
#include "MenuItem.h"

namespace Game {
  namespace UI {
    class MenuTextItem: public MenuItem {
    private:
      Font *font;
      std::string text;

    public:
      enum Theme {NORMAL, LARGE};
      MenuTextItem (
          const std::string &text = "",
          void (*select_callback) (void *select_callback_data) throw() = NULL,
          Theme theme = NORMAL) throw();

      virtual ~MenuTextItem() throw();

      void set_text (const std::string &text) throw() { this->text = text; }
      std::string get_text() const throw() { return text; }

      virtual void draw() const throw();
    };
  } // namespace UI
} // namespace Game

#endif // MENUTEXTITEM_H
