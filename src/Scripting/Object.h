/*
 * Object.h
 *
 *  Created on: 2012-01-11
 *      Author: samuel
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include "../StringTokenizer.h"
#include "../UI/GridLayout.h"
#include "Variable.h"

namespace Game {
  namespace Scripting {
    // A collection of conveniently modifiable variables.
    class Object {
    private:
      std::vector<Variable*> variables;
      UI::GridLayout *properties_widget;

    public:
      Object();
      virtual ~Object();

      // Returns a string that can be used to recreate this object.
      std::string to_string() const throw();

      void initialize (StringTokenizer &tokens) throw();

      // Upon destruction of the object, all it's variables are automatically
      // deleted.
      void consume_variable (Variable *variable);

      virtual void on_variable_changed (const std::string &name) const throw()
          {}

      // Set an existing variable and return true on success or false on
      // failure.
      bool set_variable (const std::string &name, const std::string &value)
                        throw();

      Game::UI::Widget *create_properties_widget() throw();

      // Update the properties widget with any changed variables.
      void update_properties_widget() throw();

      void destroy_properties_widget() throw();
    };
  } // namespace Scripting
} // namespace Game

#endif // OBJECT_H
