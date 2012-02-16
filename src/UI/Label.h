/*
 * Label.h
 *
 *  Created on: 2011-12-29
 *      Author: samuel
 */

#ifndef LABEL_H
#define LABEL_H

#include <string>
#include "../Font.h"
#include "Widget.h"

namespace Game {
  namespace UI {
    // Widget that displays text.
    class Label: public Widget {
    private:
      static const Font FONT;
      std::string text;

    public:
      Label (const std::string &text = "") throw();
      virtual ~Label() throw();

      void set_text (const std::string &text) throw() { this->text = text; }
      std::string get_text() const throw() { return text; }

      virtual void draw() const throw();
    };
  } // namespace UI
} // namespace Game

#endif // LABEL_H
