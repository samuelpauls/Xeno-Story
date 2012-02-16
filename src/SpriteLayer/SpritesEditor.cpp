/*
 * SpritesEditor.cpp
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#include <fstream>
#include <GL/gl.h>
#include "../Controllers/Manager.h"
#include "../Controllers/UI.h"
#include "../Controllers/Editor.h"
#include "../Map/Editor.h"
#include "SmallAvatar.h"
#include "SpriteWidget.h"
#include "SpritesEditor.h"

using Game::SpriteLayer::SpritesEditor;
using Game::Map::Editor;
using Game::UI::Frame;
using Game::UI::Menu;
using Game::UI::ScrollableContainer;

void SpritesEditor::select_sprite_in_palette (void *sprites_editor) throw() {
  SpritesEditor *casted_editor = (SpritesEditor*)sprites_editor;
  casted_editor->select_sprite (NULL);
}

void SpritesEditor::select_sprite (Sprite *sprite) throw() {
  // Unselect the previous selected sprite.
  if (selected_sprite) {
    sprite_properties_container->unconsume();
    selected_sprite->destroy_properties_widget();
  }

  // Select the provided sprite.
  selected_sprite = sprite;
  if (sprite) {
    sprite_properties_container->consume (sprite->create_properties_widget());
    palette_menu->select_by_index (Menu::NOT_SET);
  }
}

SpritesEditor::SpritesEditor (UI::Manager &ui_manager) throw():
    Map::LayerEditor (ui_manager) {
  palette_sprite_changed = false;
  added_sprite_to_world = false;

  palette_frame = new Frame ("Sprites");
  palette_frame->set_position (palette_frame_rectangle);
  Rectangle palette_menu_position = palette_frame->get_child_position();
  int item_height = palette_menu_position.h - 2 * Editor::ITEM_PADDING;
  palette_menu = new Menu (item_height, item_height, Editor::ITEM_PADDING);

  std::ifstream f ("sprites/editor_sprites.dat");
  std::string subtype;
  while (!f.eof()) {
    std::getline (f, subtype);
    SpriteWidget *sprite_widget = new SpriteWidget (subtype);
    sprite_widget->set_select_callback (select_sprite_in_palette, this);
    palette_menu->consume (sprite_widget);
  }
  f.close();

  ScrollableContainer *sc = new ScrollableContainer();
  sc->consume (palette_menu);
  palette_frame->consume (sc);
  ui_manager.consume (palette_frame);

  selected_sprite = NULL;

  previous_tick = UNSET;

  // Create the sprite properties frame.
  sprite_properties_frame = new Frame ("Sprite Properties");
  Rectangle sp_rect;
  sp_rect.h = 100;
  sp_rect.w = Frame::PADDING + Editor::MENU_ITEM_WIDTH + Frame::PADDING;
  sp_rect.x = 0;
  sp_rect.y = palette_frame->get_position().y - sp_rect.h;
  sprite_properties_frame->set_position (sp_rect);
  sprite_properties_container = new ScrollableContainer();
  sprite_properties_frame->consume (sprite_properties_container);
  ui_manager.consume (sprite_properties_frame);

  mouse_primary_click_used = false;
  mouse_primary_click_owned = false;
}

SpritesEditor::~SpritesEditor() throw() {
  // Cleanup any GUI created from the currently selected sprite.
  select_sprite (NULL);

  // Delete the sprite properties frame from the editor UI.
  ui_manager.unconsume (sprite_properties_frame);
  delete sprite_properties_frame;

  // Delete the sprite palette frame from the editor UI.
  ui_manager.unconsume (palette_frame);
  delete palette_frame;
}

void SpritesEditor::update (World &world, const Rectangle &viewport,
                            unsigned int tick) throw() {
  Controllers::UI *ui_controller = Controllers::Manager::get_ui_controller();
  Controllers::Editor *editor_controller =
      Controllers::Manager::get_editor_controller();

  if (previous_tick == UNSET) {
    previous_tick = tick;
  }

  // Select a sprite in the palette by hot key.
  if (editor_controller->is_next_brush_pressed() ||
      editor_controller->is_previous_brush_pressed()) {
    if (!palette_sprite_changed) {
      select_sprite (NULL);
      if (editor_controller->is_previous_brush_pressed()) {
        palette_menu->decrement_selection();
      } else if (editor_controller->is_next_brush_pressed()) {
        palette_menu->increment_selection();
      }
      palette_sprite_changed = true;
    }
  } else {
    palette_sprite_changed = false;
  }

  int mouse_x = ui_controller->get_x();
  int mouse_y = ui_controller->get_y();
  int world_x = viewport.x + mouse_x;
  int world_y = viewport.y + mouse_y;

  // If the mouse primary clicked in the world...
  if (ui_controller->is_primary_down()) {
    if (ui_manager.intersects (mouse_x, mouse_y)) {
      mouse_primary_click_used = true;
      mouse_primary_click_owned = false;
    } else {
      if (!mouse_primary_click_used) {
        if (palette_menu->get_selection_index() == Menu::NOT_SET) {
          select_sprite (world.get_touching_sprite (world_x, world_y));
          if (selected_sprite) {
            selected_mouse_offset_x =
                world_x - selected_sprite->get_position().x;
            selected_mouse_offset_y =
                world_y - selected_sprite->get_position().y;
          }
        } else {
          // Add a sprite to the world.
          SpriteWidget *sw = (SpriteWidget*)palette_menu->get_selected_widget();
          select_sprite (sw->get_sprite()->clone());
          selected_sprite->set_position (world_x, world_y);
          world.add (*selected_sprite);
          selected_mouse_offset_x = selected_sprite->get_position().w / 2;
          selected_mouse_offset_y = selected_sprite->get_position().h / 2;
        }

        mouse_primary_click_used = true;
        mouse_primary_click_owned = true;
      }
    }
  } else {
    mouse_primary_click_used = false;
    mouse_primary_click_owned = false;
  }

  if (selected_sprite) {
    // Drag the selected sprite with the mouse.
    if (ui_controller->is_primary_down() && mouse_primary_click_owned) {
      selected_sprite->set_position (world_x - selected_mouse_offset_x,
                                     world_y - selected_mouse_offset_y);
      selected_sprite->update_properties_widget();
    }

    if (!ui_manager.has_focus()) {
      int speed = Editor::SPEED_PER_SECOND * (tick - previous_tick) / 1000.0;

      // Move the selected sprite by hot key.
      if (editor_controller->is_scroll_left_pressed()) {
        Rectangle sprite_rect = selected_sprite->get_position();
        selected_sprite->set_position (sprite_rect.x - speed, sprite_rect.y);
        selected_sprite->update_properties_widget();
      }
      if (editor_controller->is_scroll_right_pressed()) {
        Rectangle sprite_rect = selected_sprite->get_position();
        selected_sprite->set_position (sprite_rect.x + speed, sprite_rect.y);
        selected_sprite->update_properties_widget();
      }
      if (editor_controller->is_scroll_up_pressed()) {
        Rectangle sprite_rect = selected_sprite->get_position();
        selected_sprite->set_position (sprite_rect.x, sprite_rect.y + speed);
        selected_sprite->update_properties_widget();
      }
      if (editor_controller->is_scroll_down_pressed()) {
        Rectangle sprite_rect = selected_sprite->get_position();
        selected_sprite->set_position (sprite_rect.x, sprite_rect.y - speed);
        selected_sprite->update_properties_widget();
      }

      // Scroll the world as the dragged sprite nears the edge of the viewport.
      static const int SCROLL_MAP_BORDER = 50;
      Rectangle spr_pos = selected_sprite->get_position();
      if (spr_pos.x < viewport.x + SCROLL_MAP_BORDER) {
        world.get_camera()->move (-speed, 0);
      } else if (spr_pos.get_right() > viewport.get_right() - SCROLL_MAP_BORDER) {
        world.get_camera()->move (speed, 0);
      }
      if (spr_pos.get_top() > viewport.get_top() - SCROLL_MAP_BORDER) {
        world.get_camera()->move (0, speed);
      } else if (spr_pos.y < viewport.y + SCROLL_MAP_BORDER) {
        world.get_camera()->move (0, -speed);
      }

      // Delete the selected sprite if requested by the user.
      if (editor_controller->is_delete_pressed()) {
        Sprite *s = selected_sprite;
        select_sprite (NULL);
        world.detatch_sprite (*s);
        delete s;
      }
    } // if the editor UI doesn't have focus
  } // if a sprite is selected

  previous_tick = tick;
}

void SpritesEditor::draw() throw() {
  // Draw the selected sprite rectangle.
  if (selected_sprite) {
    glColor3f (1.0F, 1.0F, 1.0F); // white
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    Rectangle spr_pos = selected_sprite->get_position();
    glRecti (spr_pos.x, spr_pos.y, spr_pos.get_right(), spr_pos.get_top());
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }
}
