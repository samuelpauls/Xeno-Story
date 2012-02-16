/*
 * String.h
 *
 *  Created on: 2012-01-10
 *      Author: samuel
 */

#ifndef STRING_H
#define STRING_H

#include <string>
#include "../UI/Widget.h"
#include "../UI/TextBox.h"
#include "Variable.h"

namespace Game {
  namespace Scripting {
    class String: public Variable {
      std::string &value;
      UI::TextBox *text_box;

      static void set_from_widget (void *property) throw();

    public:
      String (const std::string &human_name, std::string &value) throw();
      virtual ~String() throw();

      virtual bool set_value (const std::string &value) throw();

      virtual std::string get_value() const throw() { return value; }

      virtual UI::Widget *create_value_widget() throw();

      virtual void update_value_widget() throw();

      virtual void destroy_value_widget() throw();
    };
  } // namespace Scripting
} // namespace Game

#endif // STRING_H
