/*
 * MessageBox.h
 *
 *  Created on: 2011-08-24
 *      Author: samuel
 */

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <string>
#include "Animation.h"
#include "Font.h"

namespace Game {
  // Displays a message box for the player to read personal thoughts or dialog
  // from other avatars.
  class MessageBox {
  private:
    Animation *background;
    Animation *portrait;
    Animation *proceed;
    static Font text;
    std::string name;
    std::string message;

  public:
    MessageBox (const std::string &name, const std::string &message) throw();
    virtual ~MessageBox() throw();

    void update (unsigned int tick) throw();
    void draw() const throw();
  };
}

#endif // MESSAGEBOX_H
