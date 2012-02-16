/*
 * AvatarStatus.h
 *
 *  Created on: 2011-08-23
 *      Author: samuel
 */

#ifndef AVATARSTATUS_H
#define AVATARSTATUS_H

#include <string>
#include <GL/gl.h>
#include "Rectangle.h"
#include "Animation.h"
#include "Font.h"
#include "SpriteLayer/Avatar.h"

namespace Game {
  using SpriteLayer::Avatar;

  // Provides the status of a particular avatar on screen.
  class AvatarStatus {
  private:
    GLfloat x, y; // bottom-left location, origin at window's bottom-left

    Animation *background;
    static Font text;

    Rectangle health_rect;
    double health_percentage;
    bool draw_health_text;

    bool draw_armed_weapon;

    Avatar *avatar; // that information is being displayed about

  public:
    AvatarStatus (const std::string &type) throw();
    virtual ~AvatarStatus() throw();

    void monitor (Avatar *avatar) { this->avatar = avatar; }
    Avatar *get_avatar() const throw() { return avatar; }

    void update (unsigned int tick) throw();
    void draw() const throw();
  };
}

#endif // AVATARSTATUS_H
