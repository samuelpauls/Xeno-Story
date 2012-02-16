/*
 * BigAvatar.cpp
 *
 *  Created on: 2011-09-03
 *      Author: samuel
 */

#include <cmath> // for std::sqrt
#include <fstream>
#include <cassert>
#include "BigAvatar.h"

using Game::SpriteLayer::BigAvatar;

void BigAvatar::delete_state (int state) throw() {
  Common::State *s = common->states[state];
  for (unsigned int pi = 0; pi < parts.size(); ++pi) {
    delete s->part_users[pi];
  }
  delete s;
}

float BigAvatar::get_times_state_played() const throw() {
  return state_duration / common->states[get_state()]->cycle_duration;
}

void BigAvatar::avatar_draw() const throw() {
  Common::State &s = *(common->states[get_state()]);
  for (unsigned int pi = 0; pi < parts.size(); ++pi) {
    if (s.part_users[pi]) {
      Part &p = *parts[pi];
      glPushMatrix();
      glTranslatef (p.path_follower.get_x(), p.path_follower.get_y(), 0.0F);
      p.animation[get_direction()]->draw();
      glPopMatrix();
    }
  }
}

void BigAvatar::set_state (Avatar::State state) throw() {
  if (get_state() != state) {
    Common::State &new_s = *common->states[state]; // new state
    for (unsigned int pi = 0; pi < parts.size(); ++pi) { // part index
      Common::State::PartUser *part_user = new_s.part_users[pi];
      if (part_user) {
        Part &p = *parts[pi];
        PathFollower &pf = p.path_follower;
        int px = pf.get_x();
        int py = pf.get_y();

        pf.set_path (&part_user->paths[get_direction()]);

        // Check to see if there's a matching path follower from the old state's
        // part user for the new state's part user.  For example, perhaps the left
        // claw of a Giant Crab is used in the still state and also the running
        // state, but just needs to move to a slightly different location.
        Common::State &old_s = *common->states[get_state()]; // old state
        if (old_s.part_users[pi]) {
          pf.set_position (px, py);
        }
      } // if there's a part user for the part
    } // for each part

    Avatar::set_state (state);
    state_start_tick = 0;
    state_duration = 0;
  } // if state changed
}

void BigAvatar::set_direction (Direction direction) throw() {
  if (get_direction() != direction) {
    Avatar::set_direction (direction);

    Common::State &s = *common->states[get_state()];
    for (unsigned int pi = 0; pi < parts.size(); ++pi) {
      Common::State::PartUser *part_user = s.part_users[pi];
      if (part_user) {
        parts[pi]->path_follower.set_path (&part_user->paths[get_direction()]);
      }
    }
  }
}

BigAvatar::BigAvatar (const std::string &subtype) throw(): Avatar (subtype) {
  common = new Common();
  common->reference_count = 1;

  set_size (60, 120); // TODO: Don't hard code the size!

  // Load the parts.
  std::string parts_filename = "sprites/" + subtype + "/parts.dat";
  std::ifstream f (parts_filename.c_str());
  std::vector<std::string> part_names;
  while (!f.eof()) {
    Part *part = new Part();

    // Load the part's animaton.
    std::string part_filename;
    f >> part_filename;
    std::string full_part_filename = "sprites/" + subtype + '/' + part_filename;
    part->animation[RIGHT] = new Animation (full_part_filename);
    part->animation[LEFT] = new Animation (full_part_filename);
    part->animation[LEFT]->flip_horizontally();

    parts.push_back (part);

    // Store the part's name for the duration of initialisation.
    std::string part_name;
    f >> part_name;
    part_names.push_back (part_name);
  }
  f.close();

  for (int s = 0; s < STATE_COUNT; ++s) {
    Common::State *state;

    std::string state_filename =
        "sprites/" + subtype + '/' + STATE_NAMES[s] + ".dat";
    std::ifstream f (state_filename.c_str());
    if (!f) { // If animation file doesn't exist...
      state = common->states[STILL];
    } else {
      state = new Common::State();

      // Store the duration in ms of a complete cycling through all the waypoints
      // of each part user for this state.  Once all are determined, we take the
      // longest duration and use it as the state duration.
      int part_user_durations[parts.size()];
      for (unsigned int pi = 0; pi < parts.size(); ++pi) {
        part_user_durations[pi] = 0;

        state->part_users.push_back (NULL);
      }

      while (!f.eof()) {
        Common::State::PartUser *part_user = new Common::State::PartUser();

        // Connect the part user to a part.
        std::string anim_name;
        std::getline (f, anim_name); // finish the previous line
        std::getline (f, anim_name);
        bool part_user_pushed = false;
        unsigned int pi;
        for (pi = 0; pi < parts.size(); ++pi) {
          if (part_names[pi] == anim_name) {
            state->part_users[pi] = part_user;
            part_user_pushed = true;
            break;
          }
        }
        assert (part_user_pushed);

        // Load the part's general settings.
        int waypoint_count;
        f >> waypoint_count;

        // Load the waypoints for this state.
        // TODO: Remove need for ticks to speed_per_second conversion.
        int x[waypoint_count];
        int y[waypoint_count];
        int ticks[waypoint_count];
        for (int wi = 0; wi < waypoint_count; ++wi) { // waypoint index
          f >> x[wi] >> y[wi] >> ticks[wi];
        }
        for (int wi = 0; wi < waypoint_count; ++wi) { // waypoint index
          int rx;
          rx = x[wi];
          int nwi = (wi + 1) % waypoint_count;
          int dx = x[nwi] - x[wi];
          int dy = y[nwi] - y[wi];
          int dist = std::sqrt (dx * dx + dy * dy);
          unsigned int speed_per_second = dist / (ticks[wi] / 1000.0);
          part_user->paths[RIGHT].push_node (rx, y[wi], speed_per_second);
          int lx = destination_rectangle->w - rx -
              parts[pi]->animation[LEFT]->get_width();
          part_user->paths[LEFT].push_node (lx, y[wi], speed_per_second);
          part_user_durations[pi] += ticks[wi];
        }
      } // for each part in the state

      // Set the duration of this state to that of the part user with the longest
      // duration.
      state->cycle_duration = 0;
      for (unsigned int pi = 0; pi < parts.size(); ++pi) {
        if (state->cycle_duration < part_user_durations[pi]) {
          state->cycle_duration = part_user_durations[pi];
        }
      }

      f.close();
    } // if the state file exists

    common->states.push_back (state);
  } // for each state

  state_start_tick = 0;
  state_duration = 0;
}

BigAvatar::BigAvatar (const BigAvatar &big_avatar) throw():
    Avatar (big_avatar) {
  common = big_avatar.common;
  ++common->reference_count;

  // Copy the parts from the old big avatar.
  for (unsigned int p = 0; p < big_avatar.parts.size(); ++p) {
    Part *part = new Part();
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
      part->animation[d] = new Animation (*big_avatar.parts[p]->animation[d]);
    }
    parts.push_back (part);
  }

  state_start_tick = big_avatar.state_start_tick;
  state_duration = big_avatar.state_duration;
}

BigAvatar::~BigAvatar() throw() {
  // Delete all parts.
  for (unsigned int p = 0; p < parts.size(); ++p) {
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
      delete parts[p]->animation[d];
    }
    delete parts[p];
  }

  // Delete shared data once no other cloned subtypes are using it.
  if (--common->reference_count == 0) {
    // Delete the states and their particular usage of parts.
    for (int si = 1; si < STATE_COUNT; ++si) {
      if (common->states[si] != common->states[STILL]) {
        delete_state (si);
      }
    }
    delete_state (STILL);

    delete common;
  }
}

void BigAvatar::avatar_update (World *world, unsigned int tick) throw() {
  Common::State &s = *(common->states[get_state()]);

  for (unsigned int pi = 0; pi < parts.size(); ++pi) {
    if (s.part_users[pi]) {
      parts[pi]->path_follower.update (tick);
      parts[pi]->animation[get_direction()]->update (tick);
    }
  }

  if (state_start_tick == 0) {
    state_start_tick = tick;
  }
  state_duration = tick - state_start_tick;
}
