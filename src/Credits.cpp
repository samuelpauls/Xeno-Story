/*
 * Credits.cpp
 *
 *  Created on: 2011-10-11
 *      Author: samuel
 */

#include <cassert>
#include <fstream>
#include <GL/gl.h>
#include "Rectangle.h"
#include "Credits.h"

using Game::Credits;

Game::Font Credits::title_text (30);
Game::Font Credits::person_text (20);

Credits::Credits() {
  credits_height = 0;

  // Hard code Samuel's name in for the engine.  Other credits aren't hard coded
  // as the engine is flexible enough for others to create their own games too.
  Line *line = new Line();
  line->text = &title_text;
  line->string = "Programming";
  lines.push_back (line);
  credits_height += line->text->get_line_height();
  credits_height += SPACE_IN_GROUP;
  line = new Line();
  line->text = &person_text;
  line->string = "Samuel Pauls";
  lines.push_back (line);
  credits_height += line->text->get_line_height();

  std::string config_filename = "credits.dat";
  std::ifstream f (config_filename.c_str());

  Font *text = &title_text;
  while (!f.eof()) {
    std::string line_string;
    std::getline (f, line_string);

    // Remove the end of line characters.
    while (!line_string.empty() &&
           std::iscntrl (line_string[line_string.size() - 1])) {
      line_string = line_string.substr (0, line_string.size() - 1);
    }

    if (line_string.empty()) {
      text = &title_text;
    } else {
      Line *line = new Line();
      line->string = line_string;
      line->text = text;
      lines.push_back (line);
      credits_height += text->get_line_height();
      if (line->text == &title_text) {
        credits_height += SPACE_AFTER_GROUP;
      } else {
        credits_height += SPACE_IN_GROUP;
      }
      text = &person_text;
    }
  }

  f.close();

  start_tick = 0;
  first_y = 0;
  done = false;
}

Credits::~Credits() {
  // Delete all the lines.
  for (std::list<Line*>::const_iterator l = lines.begin();
       l != lines.end();
       ++l) {
    delete *l;
  }
}

void Credits::update (unsigned int tick) {
  if (start_tick == 0) {
    start_tick = tick;
  }

  static const int SPEED = 30; // pixels per second
  first_y = (tick - start_tick) / 1000.0F * SPEED;

  // If the credits have scrolled off the top of the screen, we're done.
  if (first_y - credits_height >= Window::get_height()) {
    done = true;
  }
}

void Credits::draw (Window &window) {
  // Clear the screen to black.
  glClearColor (0.0F, 0.0F, 0.0F, 1.0F);
  glClear (GL_COLOR_BUFFER_BIT);

  // Set the text color.
  glColor3f (1.0F, 1.0F, 1.0F); // white

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  // Draw the credit text.
  glTranslatef (0.0F, first_y, 0.0F);
  Line *prev_line = NULL;
  for (std::list<Line*>::const_iterator l = lines.begin();
       l != lines.end();
       ++l) {
    Line *line = *l;

    // Translate vertically for the credit line.
    glTranslatef (0.0F, -line->text->get_line_height(), 0.0F);
    if (prev_line) {
      if (line->text == &title_text) {
        glTranslatef (0.0F, -SPACE_AFTER_GROUP, 0.0F);
      } else {
        glTranslatef (0.0F, -SPACE_IN_GROUP, 0.0F);
      }
    }

    // Draw the credit line, centered.
    glPushMatrix();
    int x = (Window::get_width() - line->text->get_width (line->string)) / 2;
    glTranslatef (x, 0.0F, 0.0F);
    line->text->draw (line->string);
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix();

    prev_line = line;
  }
}
