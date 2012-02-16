/*
 * Variable.h
 *
 *  Created on: 2012-01-10
 *      Author: samuel
 */

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include "../UI/Widget.h"

namespace Game {
  namespace Scripting {
    class Object;

    class Variable {
    private:
      std::string human_name;

      // The machine name is suitable to scripts as it doesn't require quotes
      // (spaces are replaced with underscores).  It's also prettier with all
      // lower case letters.
      std::string machine_name;

      Object *object;

    public:
      Variable (const std::string &human_name) throw();

      // The destructor doesn't destroy any existing value widget.  That is the
      // job of destroy_value_widget() which ensures all the
      // create_value_widget() calls are balanced.
      virtual ~Variable() throw();

      void set_parent_object (Object *object) throw() { this->object = object; }

      std::string get_human_name() const throw() { return human_name; }

      std::string get_machine_name() const throw() { return machine_name; }

      // Returns true on success and false on failure.
      virtual bool set_value (const std::string &value) throw();

      virtual std::string get_value() const throw() = 0;

      // Be sure to call destroy_value_widget() after use.
      virtual UI::Widget *create_value_widget() throw() = 0;

      // Update the value widget with the variable's current value.
      virtual void update_value_widget() throw() = 0;

      // Destroys the widget create by create_value_widget().
      virtual void destroy_value_widget() throw() = 0;
    }; // class Variable
  } // namespace Scripting
} // namespace Game

#endif // VARIABLE_H
