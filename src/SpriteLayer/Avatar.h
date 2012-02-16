/*
 * Avatar.h
 *
 *  Created on: 2011-09-03
 *      Author: samuel
 */

#ifndef AVATAR_H
#define AVATAR_H

#include <string>
#include <list>
#include <vector>
#include "../Audio.h"
#include "../World.h"
#include "../Lighting.h"
#include "../Controllers/Avatar.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    class Weapon;

    // A character that walks, jumps, carries weapons, and so on.
    class Avatar: public Sprite {
    public:
      enum Direction {LEFT, RIGHT, DIRECTION_COUNT};
      enum WeaponMode {UNARMED, ARMED, WEAPON_MODE_COUNT};
      enum State {STILL, RUNNING, STOPPING, JUMPING, FALLING, STATE_COUNT};

    private:
      // Data shared among copied avatars.
      struct Common {
        int speed_per_second; // in pixels

        unsigned int jump_duration; // in ms
        int weight; // determines quake magnitude upon landing

        int full_health;
        Audio *hit_sound;
        int team;
        Explosion *explosion;
        std::list<PowerUp*> pickups;

        int weapon_container_count;

        int reference_count;
      } *common;

      // Movement
      State state;
      Direction direction;
      bool moving_horizontally;
      int horizontal_movement_start_x;
      unsigned int horizontal_movement_start_tick;
      int vertical_movement_start_y;
      unsigned int vertical_movement_start_tick;
      static int dist_from_falling_with_gravity (int milliseconds) throw();

      // Weapons
      WeaponMode weapon_mode;
      int weapon_index; // currently selected weapon
      struct WeaponContainer {
        std::string name;
        bool equipped;
        Weapon *weapon; // NULL if a compatible weapon hasn't yet been picked up
        Animation *holding_animations[DIRECTION_COUNT];

        // The projectile spawn point relative to the avatar is affected by how
        // the avatar holds the corresponding weapon.
        int shot_offset_x, shot_offset_y;
      } *weapon_containers;
      static Audio switch_weapon_sound;
      std::vector<Weapon*> equipped_weapons;

      int health;
      bool recently_hurt;
      static const unsigned int NOT_HURT = 0;
      unsigned int hurt_tick; // tick ms of when avatar was hurt
      bool god; // If true, this avatar can't lose health.
      std::string death_command;

      // The first controller is always a dummy of the type Controllers::Avatar,
      // so it's safe to use the current controller even if none is apparently
      // set.
      std::list <Controllers::Avatar*> controllers;

      bool blocked; // True if avatar is trying to move horizontally but blocked
                    // by the environment.

      virtual float get_times_state_played() const throw() = 0;

      bool damage_flicker;
      virtual void avatar_draw() const throw() = 0;

      void register_properties() throw();

    protected:
      static const std::string STATE_NAMES[];

      virtual void set_state (State state) throw() { this->state = state; }
      State get_state() const throw() { return state; }

      virtual void set_direction (Direction direction) throw() {
        this->direction = direction;
      }

      int get_weapon_mode() { return weapon_mode; }

      // width and height in pixels
      void set_size (int width, int height) throw();

    public:
      Avatar (const std::string &subtype) throw();
      Avatar (const Avatar &avatar) throw();
      virtual ~Avatar() throw();

      virtual void on_script_variable_changed (const std::string &name) throw();

      void push_controller (Controllers::Avatar *controller) throw() {
        controllers.push_back (controller);
      }
      // Remember to delete popped controllers that will no longer be used.
      void pop_controller() throw();
      Controllers::Avatar *get_controller() const throw() {
        return controllers.back();
      }

      // in pixels
      virtual void set_position (int x, int y) throw();

      bool is_blocked() const throw() { return blocked; }

      void set_team (int team) throw() { common->team = team; }
      int get_team() const throw() { return common->team; }

      int get_full_health() const throw() { return common->full_health; }
      void affect_full_health (int change) throw();
      int get_health() const throw() { return health; }
      void affect_health (int change);

      void affect_energy (int change) throw();

      // If god mode is set to true, this avatar can't lose health.
      void set_god (bool god) throw() { this->god = god; }

      Direction get_direction() const throw() { return direction; }

      // Allows this avatar to use a weapon that it's already capable of using.
      void pickup_weapon (Weapon &weapon) throw();

      int get_equipped_weapon_count() const throw() {
        return equipped_weapons.size();
      }

      const Weapon *get_equipped_weapon (int index) const throw() {
        return equipped_weapons[index];
      }

      // Returns the armed weapon or NULL.
      Weapon *get_armed_weapon() const throw() {
        return weapon_containers[weapon_index].weapon;
      }

      // Returns the position, relative to the avatar's bottom-left, that shots
      // are fired from by the current weapon.
      int get_shot_origin_x() const throw();
      int get_shot_origin_y() const throw();

      // Update the sprite's state.  (location, animation, etc)
      // tick is ms since program start
      void update (World *world, unsigned int tick) throw();
      virtual void avatar_update (World *world, unsigned int tick) throw() = 0;

      // Draw the sprite in its current state.
      virtual void draw (Lighting *lighting) throw();

      // Has this sprite removed from the sprites list and deleted ASAP.
      virtual void kill (World &world) throw();
    };
  } // namespace SpriteLayer
}

#endif // AVATAR_H
