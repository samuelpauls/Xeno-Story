/*
 * Avatar.h
 *
 *  Created on: 2011-08-20
 *      Author: samuel
 */

#ifndef CONTROLLER_AVATAR_H
#define CONTROLLER_AVATAR_H

namespace Game {
  namespace SpriteLayer {
    class Avatar;
  }
  class World;

  namespace Controllers {
    class Avatar {
    public:
      Avatar() throw();
      virtual ~Avatar() throw();
      virtual Avatar *clone() const throw() { return new Avatar (*this); }

      // tick in ms.
      virtual void update (const World *world,
                           const SpriteLayer::Avatar &controlled_avatar,
                           unsigned int tick) throw() {}

      virtual bool is_left_pressed() const throw() { return false; }
      virtual bool is_right_pressed() const throw() { return false; }
      virtual bool is_up_pressed() const throw() { return false; }
      virtual bool is_down_pressed() const throw() { return false; }
      virtual bool is_jump_pressed() const throw() { return false; }

      virtual bool is_weapon_previous_pressed() const throw() { return false; }
      virtual bool is_weapon_next_pressed() const throw() { return false; }
      virtual bool is_weapon_fire_pressed() const throw() { return false; }
    };
  } // Controllers
}

#endif // CONTROLLER_AVATAR_H
