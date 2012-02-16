/*
 * BigAvatar.h
 *
 *  Created on: 2011-09-03
 *      Author: samuel
 */

#ifndef SPRITES_BIGAVATAR_H
#define SPRITES_BIGAVATAR_H

#include <vector>
#include <string>
#include "../Animation.h"
#include "../Lighting.h"
#include "Path.h"
#include "PathFollower.h"
#include "Avatar.h"

namespace Game {
  namespace SpriteLayer {
    class BigAvatar: public Avatar {
    private:
      // Unique data for all the parts of this big avatar.  A given part isn't
      // necessarily used by all states.
      struct Part {
        // Some parts may have duplicate animations.  Note that the animations
        // aren't included in the common data as two different avatars may
        // indeed use the same graphics, but be at different frames.
        Animation *animation[DIRECTION_COUNT];

        PathFollower path_follower; // current location along path

        // Store things like part health here.
      };
      std::vector<Part*> parts;

      // Common data among clones.
      struct Common {
        struct State {
          // Each part may or may not have a corresponding "part user" in this
          // state.  For example, the running state of the Giant Crab has a part
          // user for the left claw.  NULL represents no part user for a given
          // part.
          struct PartUser {
            Path paths[DIRECTION_COUNT];
          };
          std::vector<PartUser*> part_users;

          int cycle_duration; // in ms
        };
        std::vector <State*> states;

        int reference_count;
      } *common;

      int state_start_tick; // in ms
      int state_duration; // in ms

      // Deletes a state of this sprite and all of it's part's waypoints.
      // state is the index of the state to delete.
      void delete_state (int state) throw();

      virtual float get_times_state_played() const throw();

      virtual void avatar_draw() const throw();

    protected:
      virtual void set_state (Avatar::State state) throw();
      virtual void set_direction (Direction direction) throw();

    public:
      BigAvatar (const std::string &subtype) throw();
      BigAvatar (const BigAvatar &big_avatar) throw();
      virtual ~BigAvatar() throw();

      Sprite *clone() const throw() { return new BigAvatar (*this); }

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start
      virtual void avatar_update (World *world, unsigned int tick) throw();
    };
  } // namespace SpriteLayer
}

#endif // SPRITES_BIGAVATAR_H
