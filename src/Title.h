/*
 * Title.h
 *
 *  Created on: 2011-10-13
 *      Author: samuel
 */

#ifndef TITLE_H
#define TITLE_H

#include <string>
#include "Font.h"
#include "Audio.h"

namespace Game {
  class Title {
  private:
    static Audio sound;
    static Font text;
    std::string string;

    unsigned int start_tick;
    bool done; // true once the acquisition dialog is finished

  public:
    Title (const std::string &string) throw();
    virtual ~Title() throw();

    // ticks in ms since the world began.
    void update (unsigned int tick) throw();

    void draw() const throw();

    // Returns true once the achievement acquisition dialog is finished.
    bool is_done() const throw() { return done; }
  };
} // namespace Game
#endif // TITLE_H
