/*
 * Sprite.h
 *
 *  Created on: 2011-08-15
 *      Author: samuel
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "../Audio.h"
#include "../Rectangle.h"
#include "../Scripting/Object.h"
#include "../UI/Widget.h"

namespace Game {
  class World;
  class Lighting;
  class Animation;
  class Window;

  namespace SpriteLayer {
    class Sprite {
    private:
      // Data shared between sprites that are clones of each other.
      struct Common {
        std::string subtype;
        Audio *spawn_sound;

        int reference_count;
      } *common;

      // Each sprite can be uniquely identified.  This could be used to refer to
      // a specific sprite when a switch sprite is activated, for example.
      std::string id;

      void set_unique_id() throw();

      // Dead sprites are removed from the sprites list and deleted as soon as
      // possible.  Consider a sprite instantiated on the stack.  It will be
      // destroyed automatically upon exit of its scope.  If we deleted that
      // sprite manually inside of update(), that would be two destructions which
      // would be bad!  Instead we set this value to have it destroyed once later.
      bool dead;

      class SpriteScriptingObject: public Scripting::Object {
      private:
        Sprite &sprite;

      public:
        SpriteScriptingObject (Sprite &sprite): sprite (sprite) {}
        ~SpriteScriptingObject() throw() {}

        virtual void on_variable_changed (const std::string &name)
                                         const throw() {
          sprite.on_script_variable_changed (name);
        }
      };

    protected:
      Rectangle *destination_rectangle;
      SpriteScriptingObject object;

      void register_properties() throw();

    public:
      Sprite (const std::string &subtype) throw();
      Sprite (const Sprite &sprite) throw(); // needed by clone
      virtual ~Sprite();

      std::string get_subtype() const throw() { return common->subtype; }

      // Copy this sprite.  The alternative would be loading it again, which
      // involves the slow hard disk.  Portions of the cloned sprite may be shared
      // so that less memory is used overall.
      virtual Sprite *clone() const throw() = 0;

      // Create a sprite of the specified subtype (ie door).  Remember to delete
      // the sprite when finished with it.
      static Sprite *create (const std::string &subtype) throw();

      std::string get_id() const throw() { return id; }

      void play_spawn_sound() throw();

      // in pixels
      virtual void set_position (int x, int y) throw();

      Rectangle get_position() const throw() { return *destination_rectangle; }

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start
      virtual void update (World *world, unsigned int tick) throw() = 0;

      // This sprite affects the provided sprite argument.
      virtual void affect (World &world, Sprite &sprite) throw() {}

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw() = 0;

      // Has this sprite removed from the sprites list and deleted ASAP.
      virtual void kill (World &world) throw() { dead = true; }

      // Returns true if the sprite should be deleted as soon as possible.
      bool is_dead() const throw() { return dead; }

      // Converts between strings and sprites.
      std::string to_string() const throw();
      static Sprite *from_string (const std::string &string) throw();

      virtual void on_script_variable_changed (const std::string &name) throw()
          {}

      // Used by the sprite editor to allow editing of this sprite's properties.
      UI::Widget *create_properties_widget() throw() {
        return object.create_properties_widget();
      }
      void update_properties_widget() throw() {
        object.update_properties_widget();
      }
      void destroy_properties_widget() throw() {
        object.destroy_properties_widget();
      }

      // Attempt to execute a command and return true if success or false on
      // failure.
      bool execute (const std::string &command) throw();
    };
  }
}

#endif // SPRITE_H
