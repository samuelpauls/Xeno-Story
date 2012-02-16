/*
 * SpritesEditor.h
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#ifndef SPRITESEDITOR_H
#define SPRITESEDITOR_H

#include "../Rectangle.h"
#include "../World.h"
#include "../UI/Manager.h"
#include "../UI/Menu.h"
#include "../UI/Frame.h"
#include "../UI/ScrollableContainer.h"
#include "../Map/LayerEditor.h"
#include "../Map/Layer.h"
#include "Sprite.h"
#include "Sprites.h"

namespace Game {
  namespace SpriteLayer {
    // Allows the user to edit a sprite layer of the map.
    class SpritesEditor: public Map::LayerEditor {
    private:
      // The palette is used to select sprites for placement in the world.
      UI::Menu *palette_menu;
      UI::Frame *palette_frame;
      bool palette_sprite_changed;

      bool added_sprite_to_world;

      // The selected sprite can be moved around and deleted.
      Sprite *selected_sprite; // in world
      int selected_mouse_offset_x, selected_mouse_offset_y;

      // Must be static because this function is used as a callback by the menu.
      static void select_sprite_in_palette (void *sprite_editor) throw();

      static const int UNSET = 0;
      int previous_tick; // in ms

      UI::Frame *sprite_properties_frame;
      UI::ScrollableContainer *sprite_properties_container;

      void select_sprite (Sprite *sprite) throw();

      bool mouse_primary_click_used;
      bool mouse_primary_click_owned;

    public:
      SpritesEditor (UI::Manager &ui_manager) throw();
      virtual ~SpritesEditor() throw();

      virtual void set_layer (Map::Layer &layer) throw() {};

      bool is_sprite_selected() const throw() { return selected_sprite; }

      // Allows mouse to select different sprites.
      // tick in ms
      void update (World &world, const Rectangle &viewport,
                   unsigned int tick) throw();

      void draw() throw();
    };
  } // Sprites namespace
}

#endif /* SPRITESEDITOR_H_ */
