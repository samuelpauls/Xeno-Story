/*
 * AI.h
 *
 *  Created on: 2011-08-21
 *      Author: samuel
 */

#ifndef CONTROLLER_AI_H
#define CONTROLLER_AI_H

#include "../World.h"
#include "../SpriteLayer/Avatar.h"
#include "Avatar.h"

namespace Game {
  namespace Controllers {
    class AI: public Avatar {
    private:
      bool left_pressed;
      bool right_pressed;
      bool jump_pressed;

      bool weapon_next;
      bool weapon_fire;

      unsigned int change_tick; // in ms

      // Set this AI controller to its default settings.
      void initialize() throw();

    public:
      AI() throw();
      AI (const AI &ai) throw();
      virtual ~AI() throw();
      virtual Avatar *clone() const throw() { return new AI (*this); }

      // tick in ms.
      void update (const World *world, const SpriteLayer::Avatar &controlled_avatar,
                   unsigned int tick) throw();

      virtual bool is_left_pressed() const throw() { return left_pressed; }
      virtual bool is_right_pressed() const throw() { return right_pressed; }
      virtual bool is_jump_pressed() const throw() { return jump_pressed; }

      virtual bool is_weapon_next_pressed() const throw() { return weapon_next; }
      virtual bool is_weapon_fire_pressed() const throw() { return weapon_fire; }
    }; // class AI
  } // namespace Controllers
} // namespace Game

#endif // CONTROLLER_AI_H
