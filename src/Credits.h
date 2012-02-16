/*
 * Credits.h
 *
 *  Created on: 2011-10-11
 *      Author: samuel
 */

#ifndef CREDITS_H
#define CREDITS_H

#include <string>
#include <list>
#include "Window.h"
#include "Font.h"

namespace Game {
  class Credits {
  private:
    // A line in the credits could either be a title or person.
    struct Line {
      std::string string;
      Font *text;
    };
    std::list<Line*> lines;

    static Font title_text;
    static Font person_text;
    static const int SPACE_IN_GROUP = 20; // in pixels
    static const int SPACE_AFTER_GROUP = 60; // in pixels

    int first_y; // y pixel of the top of the credits
    int credits_height; // in pixels
    unsigned int start_tick; // ms since credits started
    bool done; // true once all credits have scrolled

  public:
    Credits();
    virtual ~Credits();

    // tick is ms since the start of the game.
    void update (unsigned int tick);

    void draw (Window &window);

    // Returns true if the credits have finished scrolling.
    bool is_done() { return done; }
  };
} // namespace Game
#endif // CREDITS_H
