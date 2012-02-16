/*
 * Editor.cpp
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#include <typeinfo> // for typeid
#include <string>
#include <fstream>
#include "../Camera.h"
#include "../Controllers/Manager.h"
#include "../Controllers/Editor.h"
#include "../UI/MenuTextItem.h"
#include "../TileLayer/TilesEditor.h"
#include "../SpriteLayer/SpritesEditor.h"
#include "../SpriteLayer/Sprites.h"
#include "../ImageLayer/ImageLayer.h"
#include "../ImageLayer/ImageLayerEditor.h"
#include "Map.h"
#include "Editor.h"

using Game::Map::Editor;
using Game::UI::Manager;
using Game::UI::Frame;
using Game::UI::Menu;
using Game::UI::MenuTextItem;

void Editor::edit_map (void *editor) throw() {
  Editor *casted_editor = (Editor*)editor;

  if (!casted_editor->map_set_externally) {
    std::string &map_name =
      casted_editor->map_names[casted_editor->map_menu->get_selection_index()];
    casted_editor->world.set_map (map_name);
  }
}

void Editor::edit_layer (void *data) throw() {
  Editor *editor = ((EditLayerData*)data)->editor;
  Layer *layer = ((EditLayerData*)data)->layer;

  // Select the new layer.
  if (TileLayer::TileLayout *tiles = dynamic_cast<TileLayer::TileLayout*> (layer)) {
    if (!editor->layer_editor || typeid (*editor->layer_editor) != typeid (TileLayer::TilesEditor)) {
      delete editor->layer_editor;
      editor->layer_editor = new TileLayer::TilesEditor (editor->ui_manager);
    }
    editor->layer_editor->set_layer (*tiles);
  } else if (typeid (*layer) == typeid(SpriteLayer::Sprites)) {
    if (!editor->layer_editor ||
        typeid (*editor->layer_editor) != typeid (SpriteLayer::SpritesEditor)) {
      delete editor->layer_editor;
      editor->layer_editor = new SpriteLayer::SpritesEditor (editor->ui_manager);
    }
  } else if (ImageLayer::ImageLayer *image_layer =
             dynamic_cast<ImageLayer::ImageLayer*> (layer)) {
    if (!editor->layer_editor || typeid (*editor->layer_editor) !=
        typeid (ImageLayer::ImageLayerEditor)) {
      delete editor->layer_editor;
      editor->layer_editor =
          new ImageLayer::ImageLayerEditor (editor->ui_manager);
    }
    editor->layer_editor->set_layer (*image_layer);
  } else {
    delete editor->layer_editor;
    editor->layer_editor = NULL;
  }
}

Editor::Editor (World &world) throw(): world (world) {
  map = NULL;
  map_set_externally = true;

  // Load all the map names.
  std::ifstream f ("maps/index.dat");
  while (!f.eof()) {
    std::string map_name;
    std::getline(f, map_name);
    // Remove '\r' which appears at the end of some lines.
    // Linux uses '\n'
    // Windows uses '\r\n'
    map_name = map_name.substr (0, map_name.find_last_not_of ('\r') + 1);
    map_names.push_back (map_name);
  }
  f.close();

  // Place all the map names in a menu.
  Rectangle map_frame_rect;
  map_frame_rect.w = MENU_ITEM_WIDTH + Frame::PADDING * 2;
  map_frame_rect.h = MENU_ITEM_HEIGHT * map_names.size() +
                     Frame::PADDING + Frame::TOP_PADDING;
  map_frame_rect.x = Window::get_width() - map_frame_rect.w;
  map_frame_rect.y = 0;
  map_frame = new Frame ("Maps");
  map_frame->set_position (map_frame_rect);
  map_menu = new Menu (MENU_ITEM_WIDTH, MENU_ITEM_HEIGHT);
  for (unsigned int n = 0; n < map_names.size(); ++n) {
    std::string map_name = map_names[n];
    MenuTextItem *mti = new MenuTextItem (map_name);
    mti->set_select_callback (edit_map, this);
    map_menu->consume (mti);
  }
  map_frame->consume (map_menu);
  ui_manager.consume (map_frame);

  layer_frame = new Frame ("Layers");
  ui_manager.consume (layer_frame);
  layer_menu = NULL;
  layer_editor = NULL;

  previous_tick = UNSET;
}

Editor::~Editor() throw() {
  delete layer_editor; // NULL pointer deletion does nothing.

  for (std::list<EditLayerData*>::iterator d = edit_layer_datas.begin();
       d != edit_layer_datas.end();
       ++d) {
    delete *d;
  }
  edit_layer_datas.clear();
}

void Editor::set_map (Map *map) throw() {
  // Create a layer menu for the map.
  if (layer_frame->get_child_count() == 1) {
    layer_frame->unconsume();
    delete layer_menu;
    for (std::list<EditLayerData*>::iterator d = edit_layer_datas.begin();
         d != edit_layer_datas.end();
         ++d) {
      delete *d;
    }
    edit_layer_datas.clear();
  }

  Rectangle layer_frame_rect;
  layer_frame_rect.x = 0;
  layer_frame_rect.y = 0;
  layer_frame_rect.w = MENU_ITEM_WIDTH + Frame::PADDING * 2;
  layer_frame_rect.h = MENU_ITEM_HEIGHT * map->get_layer_count() +
                       Frame::PADDING + Frame::TOP_PADDING;
  layer_frame->set_position (layer_frame_rect);
  layer_menu = new Menu (MENU_ITEM_WIDTH, MENU_ITEM_HEIGHT);
  for (std::list<Layer*>::const_iterator l = map->layers.begin();
       l != map->layers.end();
       ++l) {
    Layer *layer = *l;

    std::string layer_name = layer->get_name();
    MenuTextItem *item = new MenuTextItem (layer_name);

    EditLayerData *data = new EditLayerData();
    data->editor = this;
    data->layer = layer;
    edit_layer_datas.push_back (data);
    item->set_select_callback (&edit_layer, data);

    layer_menu->consume (item);
  }
  layer_frame->consume (layer_menu);

  this->map = map;

  // Select the map in the menu.
  int map_index = 0;
  for (std::vector<std::string>::const_iterator n = map_names.begin();
       n != map_names.end();
       ++n) {
    if (*n == map->get_name()) {
      break;
    } else {
      ++map_index;
    }
  }
  map_set_externally = true;
  map_menu->select_by_index (map_index);
  map_set_externally = false;

  // Edit the first layer.
  edit_layer (edit_layer_datas.front());
}

void Editor::update (const Rectangle &viewport, unsigned int tick) throw() {
  ui_manager.update (tick);

  Controllers::Editor *editor_controller =
      Controllers::Manager::get_editor_controller();

  if (previous_tick == UNSET) {
    previous_tick = tick;
  }

  // Select the next map layer for editing by hot key.
  if (editor_controller->is_next_layer_pressed()) {
    layer_menu->increment_selection();
  } else if (editor_controller->is_previous_layer_pressed()) {
    layer_menu->decrement_selection();
  }

  if (layer_editor) {
    layer_editor->update (world, viewport, tick);
  }

  // If no sprite in the world is selected...
  SpriteLayer::SpritesEditor *sprites_editor =
      dynamic_cast<SpriteLayer::SpritesEditor*> (layer_editor);
  if (!sprites_editor || !sprites_editor->is_sprite_selected()) {
    // Scroll the camera when the corresponding hot keys are pressed.
    Camera &camera = *world.get_camera();
    int speed = SPEED_PER_SECOND * (tick - previous_tick) / 1000.0;
    if (editor_controller->is_scroll_left_pressed()) {
      camera.move (-speed, 0);
    }
    if (editor_controller->is_scroll_right_pressed()) {
      camera.move (speed, 0);
    }
    if (editor_controller->is_scroll_up_pressed()) {
      camera.move (0, speed);
    }
    if (editor_controller->is_scroll_down_pressed()) {
      camera.move (0, -speed);
    }
  }

  previous_tick = tick;
}

void Editor::draw() const throw() {
  if (layer_editor) {
    layer_editor->draw();
  }
  ui_manager.draw();
}
