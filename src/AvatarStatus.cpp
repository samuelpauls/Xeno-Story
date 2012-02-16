/*
 * AvatarStatus.cpp
 *
 *  Created on: 2011-08-23
 *      Author: samuel
 */

#include <fstream>
#include <sstream>
#include <GL/gl.h>
#include "SpriteLayer/Weapon.h"
#include "AvatarStatus.h"

using Game::AvatarStatus;

Game::Font AvatarStatus::text (10);

AvatarStatus::AvatarStatus (const std::string &type) throw() {
  std::string data_filename = "avatar_status/" + type + "/general.dat";
  std::ifstream f (data_filename.c_str());

  background = new Animation ("avatar_status/" + type + "/background.png");
  f >> x >> y;

  // Load the health rectangle's rectangle.
  f >> health_rect.x >> health_rect.y >> health_rect.w >> health_rect.h;

  f >> draw_health_text;

  f >> draw_armed_weapon;

  f.close();

  avatar = NULL;
  health_percentage = 0.0;
}

AvatarStatus::~AvatarStatus() throw() {
  delete background;
}

void AvatarStatus::update (unsigned int tick) throw() {
  background->update (tick);

  if (avatar) {
    health_percentage = (double)avatar->get_health() /
                        avatar->get_full_health();
  } else {
    health_percentage = 0.0;
  }
}

void AvatarStatus::draw() const throw() {
  glPushMatrix(); // save the position before drawing the status

  // Translate to the avatar status.
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef (x, y, 0.0F);

  background->draw();

  // Draw the health bar.
  glPushMatrix();
  glTranslatef (health_rect.x, health_rect.y, 0.0F);
  glColor3ub (10, 10, 10); // near black
  glRecti (0.0F, 0.0F, health_rect.w, health_rect.h);
  glColor3ub (64, 40, 196); // purple
  glRecti (0.0F, 0.0F, health_rect.w * health_percentage, health_rect.h);
  glPopMatrix();

  // Draw the health status text.
  if (draw_health_text) {
    std::stringstream t;
    if (avatar) {
      t << avatar->get_health() << " / " << avatar->get_full_health();
    } else {
      t << "0 / 0";
    }
    glPushMatrix();
    glTranslatef (7.0F, 8.0F, 0.0F);
    text.draw (t.str());
    glPopMatrix();
  }

  // Draw the armed weapon.
  if (draw_armed_weapon) {
    if (avatar) {
      SpriteLayer::Weapon *w = avatar->get_armed_weapon();
      if (w) {
        // Draw the weapon.
        glPushMatrix();
        glTranslatef (0.0F, -15.0F, 0.0F);
        w->draw (NULL);
        glPopMatrix();

        // Draw the weapon's status.
        std::stringstream stats;
        stats << "lvl " << w->get_level() <<
                 "   nrg " << w->get_energy() << '/' << w->get_full_energy();
        glPushMatrix();
        glTranslatef (45.0F, -12.0F, 0.0F);
        glColor3f (0.0F, 0.0F, 0.0F); // black
        text.draw (stats.str());
        glPopMatrix();
      }
    }
  }

  glPopMatrix(); // restore the position to that of before drawing the status
}
