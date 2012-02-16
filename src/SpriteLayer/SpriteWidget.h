/*
 * SpriteWidget.h
 *
 *  Created on: 2011-12-30
 *      Author: samuel
 */

#ifndef SPRITEWIDGET_H
#define SPRITEWIDGET_H

#include <string>
#include "../UI/MenuItem.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    // A widget that displays a sprite that isn't part of the game world.
    class SpriteWidget: public UI::MenuItem {
    private:
      Sprite *sprite;

    public:
      SpriteWidget (const std::string &subtype) throw();
      virtual ~SpriteWidget() throw();

      Sprite *get_sprite() const throw() { return sprite; }

      // tick is in ms
      virtual void update (unsigned int tick) throw() {
        sprite->update (NULL, tick);
      }

      virtual void draw() const throw();
    };
  } // namespace SpriteLayer
} // namespace Game

#endif // SPRITEWIDGET_H
