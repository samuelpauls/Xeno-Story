/*
 * ImageLayerEditor.cpp
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#include <fstream>
#include <GL/gl.h>
#include "../UI/ScrollableContainer.h"
#include "../Controllers/Manager.h"
#include "../Controllers/UI.h"
#include "../Controllers/Editor.h"
#include "../Map/Editor.h"
#include "ImageLayerWidget.h"
#include "ImageLayerEditor.h"

using Game::ImageLayer::ImageLayerEditor;
using Game::UI::ScrollableContainer;

void ImageLayerEditor::change_image_layer (void *image_layer_editor) throw() {
  ImageLayerEditor *casted_editor = (ImageLayerEditor*)image_layer_editor;
  ImageLayerWidget *w =
      (ImageLayerWidget*)casted_editor->menu->get_selected_widget();
  casted_editor->image_layer->set_animation (
      new Animation (*w->get_animation()), w->get_filename());
}

ImageLayerEditor::ImageLayerEditor (UI::Manager &ui_manager) throw():
    Map::LayerEditor (ui_manager) {
  int image_thumbnail_width = 0;

  menu_frame = new UI::Frame ("Images");
  menu_frame->set_position (palette_frame_rectangle);

  // Create a palette of the available images.
  static const std::string IMAGE_LAYER_PATH = "image_layers/";
  std::ifstream f ((IMAGE_LAYER_PATH + "contents.dat").c_str());
  while (!f.eof()) {
    std::string filename;
    std::getline (f, filename);
    if (filename != "") { // (last line seems to be "")
      // Create an animation for this possible image layer.
      Animation *animation = new Animation (IMAGE_LAYER_PATH + filename);

      // Now that we can get the animation's width, we can determine the palette
      // item's size, and create the menu.
      if (!image_thumbnail_width) {
        Rectangle menu_position = menu_frame->get_child_position();
        int image_thumbnail_height =
            menu_position.h - 2 * Map::Editor::ITEM_PADDING; // in pixels
        float scale = (float)image_thumbnail_height / animation->get_height();
        image_thumbnail_width = animation->get_width() * scale;
        menu = new UI::Menu (image_thumbnail_width, image_thumbnail_height,
                             Map::Editor::ITEM_PADDING);
      }

      // Add this image layer to the palette.
      ImageLayerWidget *ilw = new ImageLayerWidget (animation, filename);
      ilw->set_select_callback (change_image_layer, this);
      menu->consume (ilw);
    }
  }
  f.close();

  toggled = false;

  ScrollableContainer *sc = new ScrollableContainer();
  sc->consume (menu);
  menu_frame->consume (sc);
  ui_manager.consume (menu_frame);
}

ImageLayerEditor::~ImageLayerEditor() throw() {
  ui_manager.unconsume (menu_frame);
  delete menu_frame;
}

void ImageLayerEditor::set_layer (Map::Layer &layer) throw() {
  image_layer = dynamic_cast<ImageLayer*> (&layer);

  // Set the menu selection to the new layer's image.
  for (int i = 0; i < menu->get_item_count(); ++i) {
    ImageLayerWidget *w = (ImageLayerWidget*)menu->get_widget (i);
    if (w->get_filename() == image_layer->get_animation_filename()) {
      menu->select_by_index (i);
    }
  }
}

void ImageLayerEditor::update (World &world, const Rectangle &viewport,
                               unsigned int tick) throw() {
  Controllers::UI *ui_controller = Controllers::Manager::get_ui_controller();
  Controllers::Editor *editor_controller =
      Controllers::Manager::get_editor_controller();

  // Select another image layer by hot key.
  if (editor_controller->is_next_brush_pressed() ||
      editor_controller->is_previous_brush_pressed()) {
    if (!toggled) {
      // Advance the image.
      if (editor_controller->is_next_brush_pressed()) {
        menu->increment_selection();
      } else if (editor_controller->is_previous_brush_pressed()) {
        menu->decrement_selection();
      }

      toggled = true;
    }
  } else {
    toggled = false;
  }
}
