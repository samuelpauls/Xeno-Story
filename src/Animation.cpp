/*
 * Animation.cpp
 *
 *  Created on: 2011-08-10
 *      Author: samuel
 */

#include <fstream>
#include <SFML/Graphics.hpp>
#include "Animation.h"

using Game::Animation;

Animation::Animation (const std::string &graphic_filename) throw() {
  common = new Common();
  common->reference_count = 1;

  std::size_t dot_index = graphic_filename.rfind ('.');
  std::string config_filename = graphic_filename.substr (0, dot_index) + ".dat";
  std::ifstream f (config_filename.c_str());

  if (f) { // If the config file exists and could be opened...
    f >> common->frame_count;
    f >> common->frame_duration; // in ms

    std::string mode_string;
    f >> mode_string;
    if (mode_string == "forward") {
      common->mode = FORWARD;
    } else {
      common->mode = BACKWARD;
    }

    f >> offset_x >> offset_y;

    f.close();
  } else {
    // No configuration file, so treat the animation graphic as a single frame.
    common->frame_count = 1;
    common->frame_duration = 1000; // in ms
    common->mode = FORWARD;
    offset_x = 0;
    offset_y = 0;
  }

  // Load the PNG into the image buffer.
  sf::Image sfml_image;
  sfml_image.LoadFromFile (graphic_filename);
  common->animation_width = sfml_image.GetWidth();
  common->animation_height = sfml_image.GetHeight();

  // Give the PNG to OpenGL.
  glGenTextures (1, &common->texture); // generate the OpenGL texture
  glBindTexture (GL_TEXTURE_2D, common->texture); // set active OpenGL texture
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
                common->animation_width, common->animation_height,
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                sfml_image.GetPixelsPtr());

  // When applying the bound texture to a face, use the texel nearest to the
  // corresponding pixel of the face.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  source_rectangle.x = 0;
  source_rectangle.y = 0;
  source_rectangle.w = common->animation_width / common->frame_count;
  source_rectangle.h = common->animation_height;
  frame_index = -1; // invalid frame number to start

  flip_x = false;
}

Animation::Animation (const Animation &animation) throw() {
  common = animation.common;
  ++common->reference_count;

  frame_index = animation.frame_index;

  source_rectangle = animation.source_rectangle;
  offset_x = animation.offset_x;
  offset_y = animation.offset_y;

  start_tick = animation.start_tick;
  elapsed_ticks = animation.elapsed_ticks;

  flip_x = animation.flip_x;
}

Animation::~Animation() throw() {
  // Release the shared data when no other animations are using it.
  if (--common->reference_count == 0) {
    glDeleteTextures (1, &common->texture);
    delete common;
  }
}

void Animation::update (unsigned int tick) throw() {
  elapsed_ticks = tick - start_tick;
  int new_frame_index;

  if (common->mode == FORWARD) {
    new_frame_index =
        (elapsed_ticks / common->frame_duration) % common->frame_count;
  } else {
    new_frame_index = (common->frame_count - 1) -
        ((elapsed_ticks / common->frame_duration) % common->frame_count);
  }

  if (new_frame_index != frame_index) {
    source_rectangle.x = new_frame_index * source_rectangle.w;
    frame_index = new_frame_index;
  }
}

void Animation::rewind (unsigned int tick) throw() {
  start_tick = tick;
  update (tick);
}

void Animation::draw() throw() {
  glPushAttrib (GL_TRANSFORM_BIT);

  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();
  glTranslatef (offset_x, offset_y, 0.0F);

  if (flip_x) {
    glTranslatef (get_width() / 2, 0.0F, 0.0F);
    glScalef (-1.0F, 1.0F, 1.0F);
    glTranslatef (-get_width() / 2, 0.0F, 0.0F);
  }

  // Scale the texture from [0, 1] coordinates to pixel coordinates and
  // translate it such that the source rectangle is at the texture's origin.
  glMatrixMode (GL_TEXTURE);
  glLoadIdentity();
  glScalef (1.0F / common->animation_width,
            1.0F / common->animation_height,
            1.0F);
  glTranslatef (source_rectangle.x, source_rectangle.y, 0.0F);

  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, common->texture);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Draw the texture on the model at the destination rectangle.
  glBegin (GL_QUADS);
    // Bottom-left of the model.
    glTexCoord2i (0, 0);
    glVertex3i (0, 0, 0);

    // Top-left of the model.
    glTexCoord2i (0, -source_rectangle.h);
    glVertex3i (0, source_rectangle.h, 0);

    // Top-right of the model.
    glTexCoord2i (source_rectangle.w, -source_rectangle.h);
    glVertex3i (source_rectangle.w, source_rectangle.h, 0);

    // Bottom-right of the model.
    glTexCoord2i (source_rectangle.w, 0);
    glVertex3i (source_rectangle.w, 0, 0);
  glEnd();

  glDisable (GL_TEXTURE_2D);

  glLoadIdentity(); // texture matrix

  glMatrixMode (GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();
}

bool Animation::is_done() const throw() {
  return elapsed_ticks >=
      (unsigned int)common->frame_count * (unsigned int)common->frame_duration;
}

float Animation::get_times_played() const throw() {
  return elapsed_ticks / ((float)common->frame_count * common->frame_duration);
}
