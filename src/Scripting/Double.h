/*
 * Double.h
 *
 *  Created on: 2012-01-17
 *      Author: samuel
 */

#ifndef DOUBLE_H
#define DOUBLE_H

#include <string>
#include "../UI/Widget.h"
#include "../UI/TextBox.h"
#include "Variable.h"

namespace Game {
  namespace Scripting {
    class Double: public Variable {
      double &value;
      UI::TextBox *text_box;

      static void set_from_widget (void *property) throw();

    public:
      Double (const std::string &human_name, double &value) throw();
      virtual ~Double() throw();

      virtual bool set_value (const std::string &value) throw();

      virtual std::string get_value() const throw();

      virtual UI::Widget *create_value_widget() throw();

      virtual void update_value_widget() throw();

      virtual void destroy_value_widget() throw();
    };
  } // namespace Scripting
} // namespace Game

#endif // DOUBLE_H
