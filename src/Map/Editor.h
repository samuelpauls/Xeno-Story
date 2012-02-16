/*
 * Editor.h
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#ifndef MAP_EDITOR_H
#define MAP_EDITOR_H

#include <list>
#include <vector>
#include "../Rectangle.h"
#include "../World.h"
#include "../UI/Manager.h"
#include "../UI/Frame.h"
#include "../UI/Menu.h"
#include "Layer.h"

using Game::UI::Menu;

namespace Game {
  namespace Map {
    class Map;
    class LayerEditor;

    // An editor for a map that can be instantiated on demand and used to cycle
    // through and instantiate editing for individual layers.
    class Editor {
    private:
      World &world;
      UI::Manager ui_manager;

      Map *map;
      std::vector<std::string> map_names;
      Menu *map_menu;
      UI::Frame *map_frame;
      bool map_set_externally;

      LayerEditor *layer_editor; // Editor for the current layer.
      Menu *layer_menu;
      UI::Frame *layer_frame;
      struct EditLayerData {
        Editor *editor;
        Layer *layer;
      };
      std::list<EditLayerData*> edit_layer_datas;

      static const int UNSET = 0;
      int previous_tick; // in ms

      // Edit the selected map.
      // static because the menu item callback that uses it can't call instance
      // methods
      static void edit_map (void *editor) throw();

      // Start editing the current map layer.
      // static because the menu item callback that uses it can't call instance
      // methods
      static void edit_layer (void *data) throw();

    public:
      static const int ITEM_PADDING = 4; // in pixels
      static const int MENU_ITEM_WIDTH = 110; // in pixels
      static const int MENU_ITEM_HEIGHT = 20; // in pixels

      static const int SPEED_PER_SECOND = 200; // in pixels

      Editor (World &world) throw();
      virtual ~Editor() throw();

      void set_map (Map *map) throw();

      // tick is in ms
      void update (const Rectangle &viewport, unsigned int tick) throw();

      void draw() const throw();
    };
  } // Map namespace
}

#endif // MAP_EDITOR_H
