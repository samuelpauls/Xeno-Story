/*
 * SmallAvatar.h
 *
 *  Created on: 2011-08-13
 *      Author: samuel
 */

#ifndef SMALLAVATAR_H
#define SMALLAVATAR_H

#include <string>
#include "../Animation.h"
#include "../World.h"
#include "Avatar.h"

class World;

namespace Game {
  namespace SpriteLayer {
    // A sprite that interacts with the game world.
    class SmallAvatar: public Avatar {
    private:
      // This array of animation pointers contains avatar animations for each
      // state of both directions and of both weapon modes (armed and unarmed).
      // If an animation file doesn't exist, use the next best animation.  The
      // next best animation for a state that doesn't have its own animations is
      // the still state animations.  If there are no animations for the armed
      // mode, we just use the unarmed animations.
      Animation **animations;

      Animation *current_animation;

      static const int ANIMATION_COUNT =
          DIRECTION_COUNT * STATE_COUNT * WEAPON_MODE_COUNT;

      // Returns the index of a particular avatar animation.
      int get_animation_index (int direction,
                               int state, int weapon_mode) const throw();

      virtual float get_times_state_played() const throw() {
        return current_animation->get_times_played();
      }

      virtual void avatar_draw() const throw();

    public:
      SmallAvatar (const std::string &subtype) throw();
      SmallAvatar (const SmallAvatar &small_avatar) throw();
      virtual ~SmallAvatar() throw();

      Sprite *clone() const throw() { return new SmallAvatar (*this); }

      // Called by Avatar::Update.
      // tick is ms since program start
      void avatar_update (World *world, unsigned int tick) throw();
    }; // class SmallAvatar
  } // namespace SpriteLayer
} // namespace Game

#endif // SMALLAVATAR_H
