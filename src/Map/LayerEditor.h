/*
 * LayerEditor.h
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#ifndef MAP_LAYEREDITOR_H
#define MAP_LAYEREDITOR_H

#include "../Rectangle.h"
#include "../World.h"
#include "../UI/Manager.h"

namespace Game {
  namespace Map {
    // Base class of all layer editors.  Allows Map::Editor to generically work
    // with different types of layer editors, such as sprite and tile editors.
    class LayerEditor {
    protected:
      Rectangle palette_frame_rectangle;
      UI::Manager &ui_manager;

    public:
      LayerEditor (UI::Manager &ui_manager) throw();
      virtual ~LayerEditor() throw();

      virtual void set_layer (Layer &layer) throw() = 0;

      // tick in ms.
      virtual void update (World &world, const Rectangle &viewport,
                           unsigned int tick) throw() = 0;

      virtual void draw() throw() = 0;
    };
  } // namespace Map
}

#endif // MAP_LAYEREDITOR_H
