/*
 * TilesEditor.cpp
 *
 *  Created on: 2011-08-17
 *      Author: samuel
 */

#include <GL/gl.h>
#include "../Controllers/Manager.h"
#include "../Controllers/UI.h"
#include "../UI/MenuTextItem.h"
#include "../Map/Editor.h"
#include "TilesEditor.h"

using Game::TileLayer::TilesEditor;

Game::Font TilesEditor::tileset_name_text (10);

void TilesEditor::set_tileset (void *tiles_editor) throw() {
  TilesEditor *casted_editor = (TilesEditor*)tiles_editor;

  Tileset *tileset = casted_editor->tiles->get_tileset (
      casted_editor->tilesets_menu->get_selection_index());

  Rectangle palette_frame_rect;
  palette_frame_rect.w = tileset->get_tiles_wide() * tileset->get_tile_size() +
      UI::Frame::PADDING * 2;
  palette_frame_rect.h = tileset->get_tiles_high() * tileset->get_tile_size() +
      UI::Frame::PADDING + UI::Frame::TOP_PADDING;
  palette_frame_rect.x = 0;
  palette_frame_rect.y = Window::get_height() - palette_frame_rect.h;
  casted_editor->palette_frame->set_position (palette_frame_rect);
  casted_editor->tile_palette->set_position (
      casted_editor->palette_frame->get_child_position());

  casted_editor->tile_palette->set_tileset (tileset);
}

TilesEditor::TilesEditor (UI::Manager &ui_manager) throw():
    Map::LayerEditor (ui_manager) {
  grid_x = 0;
  grid_y = 0;
  tile_hover_rectangle = NULL;

  tilesets_frame = new UI::Frame ("Tilesets");
  ui_manager.consume (tilesets_frame);
  tilesets_menu = NULL;

  tile_palette = new TilesetPalette();
  palette_frame = new UI::Frame ("Tiles");
  ui_manager.consume (palette_frame);
  palette_frame->consume (tile_palette);
}

TilesEditor::~TilesEditor() throw() {
  ui_manager.unconsume (tilesets_frame);
  delete tilesets_frame;
  delete tile_hover_rectangle;
  ui_manager.unconsume (palette_frame);
  delete palette_frame;
}

void TilesEditor::set_layer (Map::Layer &layer) throw() {
  tiles = dynamic_cast <TileLayout*> (&layer);

  // When changing the tile layer, it's possible a new map with different
  // tilesets is being loaded so we recreate the tilesets menu.
  if (tilesets_frame->get_child_count() > 0) {
    tilesets_frame->unconsume();
    delete tilesets_menu;
  }
  Rectangle tilesets_frame_position;
  tilesets_frame_position.w =
      Map::Editor::MENU_ITEM_WIDTH + UI::Frame::PADDING * 2;
  tilesets_frame_position.h =
      Map::Editor::MENU_ITEM_HEIGHT * tiles->get_tileset_count() +
      UI::Frame::PADDING + UI::Frame::TOP_PADDING;
  tilesets_frame_position.x = Window::get_width() - tilesets_frame_position.w;
  tilesets_frame_position.y = Window::get_height() - tilesets_frame_position.h;
  tilesets_frame->set_position (tilesets_frame_position);
  tilesets_menu = new UI::Menu (Map::Editor::MENU_ITEM_WIDTH,
                                Map::Editor::MENU_ITEM_HEIGHT);
  for (int t = 0; t < tiles->get_tileset_count(); ++t) {
    std::string tileset_name = tiles->get_tileset (t)->get_name();
    UI::MenuTextItem *mti = new UI::MenuTextItem (tileset_name);
    mti->set_select_callback (set_tileset, this);
    tilesets_menu->consume (mti);
  }
  tilesets_frame->consume (tilesets_menu);

  // Select the first tileset in this newly selected map layer.
  tilesets_menu->select_by_index (0);
}

void TilesEditor::update (World &world, const Rectangle &viewport,
                          unsigned int tick) throw() {
  Controllers::UI *ui_controller =
      Controllers::Manager::get_ui_controller();
  int mouse_x = ui_controller->get_x();
  int mouse_y = ui_controller->get_y();
  int tile_size = tiles->get_tile_size();
  delete tile_hover_rectangle;
  tile_hover_rectangle = NULL; // not set by delete

  grid_x = tiles->get_tile_size() - viewport.x % tiles->get_tile_size();
  grid_y = tiles->get_tile_size() - viewport.y % tiles->get_tile_size();

  // If the mouse is in the world...
  if (!ui_manager.intersects (mouse_x, mouse_y)) {
    // Turn the mouse coordinates into world coordinates.
    int world_x = viewport.x + mouse_x;
    int world_y = viewport.y + mouse_y;

    // If the mouse is over a possible tile location in the map...
    if (world_x >= 0 &&
        world_x < tiles->get_tiles_wide() * tile_size &&
        world_y >= 0 &&
        world_y < tiles->get_tiles_high() * tile_size) {
      // Calculate the tile index underneath the mouse's world coordinates.
      int tile_x = world_x / tile_size;
      int tile_y = world_y / tile_size;

      // Set the tile highlight rectangle at the index.
      tile_hover_rectangle = new Rectangle();
      tile_hover_rectangle->x = tile_x * tile_size;
      tile_hover_rectangle->y = tile_y * tile_size;
      tile_hover_rectangle->w = tile_size;
      tile_hover_rectangle->h = tile_size;

      if (ui_controller->is_primary_down()) {
        // Place a tile in the map.
        tiles->set_tile (tile_x, tile_y, tile_palette->get_selected_tile());
      } else if (ui_controller->is_secondary_down()) {
        // Select the tile from the map.
        Tile *tile = tiles->get_tile (tile_x, tile_y);
        tile_palette->select_tile (tile);
      }
    } // if possible tile in the map was clicked
  } // if the mouse is in the map
}

void TilesEditor::draw() throw() {
  // Draw a tile grid.
  glPushMatrix();
  glLoadIdentity();
  glColor4ub (224, 224, 224, 100); // translucent light gray
  glBegin (GL_LINES);
  for (int y = grid_y; y < Window::get_height(); y += tiles->get_tile_size()) {
    glVertex2i (0, y);
    glVertex2i (Window::get_width(), y);
  }
  for (int x = grid_x; x < Window::get_width(); x += tiles->get_tile_size()) {
    glVertex2i (x, 0);
    glVertex2i (x, Window::get_height());
  }
  glEnd();
  glPopMatrix();

  // Draw a box around the tile that the user is hovering over.
  if (tile_hover_rectangle) {
    glColor3ub (156, 120, 152); // light purple
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    glRecti (tile_hover_rectangle->x,
             tile_hover_rectangle->y,
             tile_hover_rectangle->get_right(),
             tile_hover_rectangle->get_top());
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }
}
