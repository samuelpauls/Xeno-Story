/*
 * Player.h
 *
 *  Created on: 2011-08-11
 *      Author: samuel
 */

#ifndef CONTROLLER_PLAYER_H
#define CONTROLLER_PLAYER_H

#include "Avatar.h"

namespace Game {
  namespace Controllers {
    // Responsible for accepting player input.
    class Player: public Avatar {
      friend class Manager;

    private:
      static bool left_pressed;
      static bool right_pressed;
      static bool up_pressed;
      static bool down_pressed;
      static bool jump_pressed;

      static bool weapon_previous;
      static bool weapon_next;
      static bool weapon_fire;

    public:
      Player() throw();
      virtual ~Player() throw();
      virtual Avatar *clone() const throw() { return new Player (*this); }

      virtual bool is_left_pressed() const throw() { return left_pressed; }
      virtual bool is_right_pressed() const throw() { return right_pressed; }
      virtual bool is_up_pressed() const throw() { return up_pressed; }
      virtual bool is_down_pressed() const throw() { return down_pressed; }
      virtual bool is_jump_pressed() const throw() { return jump_pressed; }

      virtual bool is_weapon_previous_pressed() const throw() { return weapon_previous; }
      virtual bool is_weapon_next_pressed() const throw() { return weapon_next; }
      virtual bool is_weapon_fire_pressed() const throw() { return weapon_fire; }
    };
  } // Controllers
}

#endif // CONTROLLER_PLAYER_H
