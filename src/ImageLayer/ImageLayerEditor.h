/*
 * ImageLayerEditor.h
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#ifndef IMAGELAYEREDITOR_H
#define IMAGELAYEREDITOR_H

#include "../Rectangle.h"
#include "../World.h"
#include "../UI/Manager.h"
#include "../UI/Menu.h"
#include "../UI/Frame.h"
#include "../Map/LayerEditor.h"
#include "../Map/Layer.h"
#include "ImageLayer.h"

namespace Game {
  namespace ImageLayer {
    // Allows the user to edit an image layer on the map.
    class ImageLayerEditor: public Map::LayerEditor {
    private:
      ImageLayer *image_layer;
      bool toggled;
      UI::Frame *menu_frame;
      UI::Menu *menu;

      // Changes the image layer such that it matches a new selection.
      // Must be static because this function is used as a callback by the menu.
      static void change_image_layer (void *image_layer_editor) throw();

    public:
      ImageLayerEditor (UI::Manager &ui_manager) throw();
      virtual ~ImageLayerEditor() throw();

      virtual void set_layer (Map::Layer &layer) throw();

      // Allows mouse to select different sprites.
      // tick is in ms.
      void update (World &world, const Rectangle &viewport,
                   unsigned int tick) throw();

      void draw() throw() {}
    }; // class ImageLayerEditor
  } // namespace ImageLayer
} // namespace Game

#endif // IMAGELAYEREDITOR_H
