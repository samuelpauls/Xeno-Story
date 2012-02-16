/*
 * Achievement.h
 *
 *  Created on: 2011-10-13
 *      Author: samuel
 */

#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <string>
#include "Animation.h"
#include "Audio.h"
#include "Font.h"

namespace Game {
  class Achievement {
  private:
    static Audio get_sound;
    static Animation *background;
    static Font text;
    Animation *icon;
    std::string description;
    unsigned int start_tick;
    bool done; // true when display duration is reached

  public:
    Achievement (const std::string &type) throw();
    virtual ~Achievement() throw();

    // ticks in ms since the world began.
    void update (unsigned int tick) throw();

    void draw() const throw();

    // Returns true once the achievement acquisition dialog is finished.
    bool is_done() const throw() { return done; }
  };
} // namespace Game
#endif // ACHIEVEMENT_H
