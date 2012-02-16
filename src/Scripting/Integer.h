/*
 * Integer.h
 *
 *  Created on: 2012-01-10
 *      Author: samuel
 */

#ifndef INTEGER_H
#define INTEGER_H

#include <string>
#include "../UI/Widget.h"
#include "../UI/TextBox.h"
#include "Variable.h"

namespace Game {
  namespace Scripting {
    class Integer: public Variable {
      int &value;
      UI::TextBox *text_box;

      static void set_from_widget (void *property) throw();

    public:
      Integer (const std::string &human_name, int &value) throw();
      virtual ~Integer() throw();

      virtual bool set_value (const std::string &value) throw();

      virtual std::string get_value() const throw();

      virtual UI::Widget *create_value_widget() throw();

      virtual void update_value_widget() throw();

      virtual void destroy_value_widget() throw();
    };
  } // namespace Scripting
} // namespace Game

#endif // INTEGER_H
