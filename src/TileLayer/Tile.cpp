/*
 * Tile.cpp
 *
 *  Created on: 2011-12-19
 *      Author: samuel
 */

#include <GL/gl.h>
#include "Tileset.h"
#include "Tile.h"

using Game::TileLayer::Tile;

Tile::Tile (int tile_index, Tileset *tileset, const Rectangle &source_rectangle,
            bool dark, bool destructable, bool deadly) throw() {
  index = tile_index;
  this->tileset = tileset;
  this->source_rectangle = source_rectangle;
  this->dark = dark;
  this->destructable = destructable;
  this->deadly = deadly;

  x_index = source_rectangle.x / tileset->get_tile_size();
  y_index = (tileset->get_tiles_high() - 1) -
      source_rectangle.y / tileset->get_tile_size();
}

Tile::~Tile() throw() {}

void Tile::draw() const throw() {
  glPushAttrib (GL_TRANSFORM_BIT); // save matrix mode

  // Scale the texture from [0, 1] coordinates to pixel coordinates and
  // translate it such that the source rectangle is at the texture's origin.
  glMatrixMode (GL_TEXTURE);
  glLoadIdentity();
  glScalef (1.0F / tileset->texture_width,
            1.0F / tileset->texture_height,
            1.0F);
  glTranslatef (source_rectangle.x, source_rectangle.y, 0.0F);

  // Draw the texture on the model at the destination rectangle.
  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, tileset->texture);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBegin (GL_QUADS);
    // Bottom-left of the model.
    glTexCoord2i (0, source_rectangle.h);
    glVertex3i (0, 0, 0);

    // Top-left of the model.
    glTexCoord2i (0, 0);
    glVertex3i (0, source_rectangle.h, 0);

    // Top-right of the model.
    glTexCoord2i (source_rectangle.w, 0);
    glVertex3i (source_rectangle.w, source_rectangle.h, 0);

    // Bottom-right of the model.
    glTexCoord2i (source_rectangle.w, source_rectangle.h);
    glVertex3i (source_rectangle.w, 0, 0);
  glEnd();
  glDisable (GL_TEXTURE_2D);

  glLoadIdentity();

  glPopAttrib(); // restore previous matrix mode
}

bool Tile::is_pixel_solid (int x, int y) const throw() {
  // TODO: Try to use video memory instead of keeping a duplicate SFML image around.
  //unsigned char tile_alphas[tileset->texture_width * tileset->texture_height];
  //glBindTexture (GL_TEXTURE_2D, tileset->texture);
  // glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_ALPHA, tile_x_pixel, tile_y_pixel, 1, 1, 0);
  //glGetTexImage (GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tile_alphas);

  //       unsigned char tile_alpha =
  // tile_alphas[tile_y_pixel * tileset->texture_width + tile_x_pixel];

  return tileset->sfml_image.GetPixel (source_rectangle.x + x,
                                       source_rectangle.y + y).a > 0;
}
