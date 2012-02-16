/*
 * Boolean.h
 *
 *  Created on: 2012-01-11
 *      Author: samuel
 */

#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <string>
#include "../UI/Toggle.h"
#include "Variable.h"

namespace Game {
  namespace Scripting {
    class Boolean: public Variable {
      bool &value;
      UI::Toggle *toggle;

      static void set_from_widget (void *property) throw();

    public:
      Boolean (const std::string &human_name, bool &value) throw();
      virtual ~Boolean() throw();

      virtual bool set_value (const std::string &value) throw();

      virtual std::string get_value() const throw();

      virtual UI::Widget *create_value_widget() throw();

      virtual void update_value_widget() throw();

      virtual void destroy_value_widget() throw();
    }; // class Boolean
  } // namespace Scripting
} // namespace Game

#endif // BOOLEAN_H
