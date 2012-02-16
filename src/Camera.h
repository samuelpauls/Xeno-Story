/*
 * Camera.h
 *
 *  Created on: 2011-08-16
 *      Author: samuel
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "Rectangle.h"
#include "SpriteLayer/Sprite.h"

namespace Game {
  using SpriteLayer::Sprite;

  // Responsible for deciding what the viewport into the environment is.
  class Camera {
  private:
    Rectangle viewport;
    const Sprite *follow_sprite; // if NULL, then not following any sprite

  public:
    Camera (int viewport_width, int viewport_height) throw();
    virtual ~Camera() throw();

    void follow (const Sprite *sprite) throw() { follow_sprite = sprite; }

    // Moves the camera by x and y pixels.
    void move (int x, int y) throw();

    void update() throw();
    Rectangle get_viewport() const throw() { return viewport; }

    void translate_view() throw();
  };
}

#endif // CAMERA_H
