/*
 * NineSlice.cpp
 *
 *  Created on: 2011-12-21
 *      Author: samuel
 */

#include <fstream>
#include <SFML/Graphics.hpp>
#include "NineSlice.h"

using Game::NineSlice;

NineSlice::NineSlice() throw() {
  common = new Common();
  common->reference_count = 1;

  static const std::string THEME_FILENAME = "ui/frame.png";

  // Load the PNG into the image buffer.
  sf::Image sfml_image;
  sfml_image.LoadFromFile (THEME_FILENAME);
  common->texture_width = sfml_image.GetWidth();
  common->texture_height = sfml_image.GetHeight();

  // Give the PNG to OpenGL.
  glGenTextures (1, &common->texture); // generate the OpenGL texture
  glBindTexture (GL_TEXTURE_2D, common->texture); // set active OpenGL texture
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
                common->texture_width, common->texture_height,
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                sfml_image.GetPixelsPtr());

  // When applying the bound texture to a face, use the texel nearest to the
  // corresponding pixel of the face.
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Load the nine-slice's cut lines.
  std::size_t dot_index = THEME_FILENAME.rfind ('.');
  std::string data_filename = THEME_FILENAME.substr (0, dot_index) + ".dat";
  std::ifstream f (data_filename.c_str());
  f >> common->texture_middle.x >> common->texture_middle.y >>
       common->texture_middle.w >> common->texture_middle.h;
  f.close();

  // Source seams originate from the top-left of the texture.
  common->src_seam_left1 = common->texture_middle.x - Common::PADDING;
  common->src_seam_left2 = common->texture_middle.x;
  common->src_seam_right1 = common->texture_middle.get_right();
  common->src_seam_right2 =
      common->texture_middle.get_right() + Common::PADDING;
  common->src_seam_top1 = common->texture_middle.y - Common::PADDING;
  common->src_seam_top2 = common->texture_middle.y;
  common->src_seam_bottom1 =
      common->texture_middle.y + common->texture_middle.h;
  common->src_seam_bottom2 =
      common->texture_middle.y + common->texture_middle.h + Common::PADDING;
}

NineSlice::NineSlice (const NineSlice &nine_slice) throw() {
  common = nine_slice.common;
  ++common->reference_count;

  width = nine_slice.width;
  height = nine_slice.height;
  dest_seam_left = nine_slice.dest_seam_left;
  dest_seam_right = nine_slice.dest_seam_right;
  dest_seam_top = nine_slice.dest_seam_top;
  dest_seam_bottom = nine_slice.dest_seam_bottom;
}

NineSlice::~NineSlice() throw() {
  if (--common->reference_count == 0) {
    glDeleteTextures (1, &common->texture);
    delete common;
  }
}

void NineSlice::set_size (int width, int height) throw() {
  this->width = width;
  this->height = height;

  // Destination seams originate from the bottom-left of the destination
  // rectangle.
  dest_seam_left = common->texture_middle.x - Common::PADDING;
  dest_seam_right = width - (common->texture_width -
      common->texture_middle.get_right() - Common::PADDING);
  dest_seam_top = height - (common->texture_middle.y - Common::PADDING);
  dest_seam_bottom = common->texture_height - common->texture_middle.y -
      common->texture_middle.h - Common::PADDING;
}

void NineSlice::draw() const throw() {
  glPushAttrib (GL_TRANSFORM_BIT); // save the current matrix mode

  // Scale the texture from [0, 1] coordinates to pixel coordinates and
  // translate it such that the source rectangle is at the texture's origin.
  glMatrixMode (GL_TEXTURE);
  glLoadIdentity();
  glScalef (1.0F / common->texture_width, 1.0F / common->texture_height, 1.0F);

  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, common->texture);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Draw the slices.
  // When drawing there's occasionally a one pixel gap before the right and
  // bottom sections, so we always add one pixel to the middle sections' width.
  // If it's not needed, the right sections draw on top anyway.
  glBegin (GL_QUADS);
    // Bottom-left slice.
    glTexCoord2i (0, common->texture_height);
    glVertex3i (0, 0, 0);
    glTexCoord2i (0, common->src_seam_bottom2);
    glVertex3i (0, dest_seam_bottom, 0);
    glTexCoord2i (common->src_seam_left1, common->src_seam_bottom2);
    glVertex3i (dest_seam_left, dest_seam_bottom, 0);
    glTexCoord2i (common->src_seam_left1, common->texture_height);
    glVertex3i (dest_seam_left, 0, 0);

    // Bottom-middle slice.
    glTexCoord2i (common->src_seam_left2, common->texture_height);
    glVertex3i (dest_seam_left, 0, 0);
    glTexCoord2i (common->src_seam_left2, common->src_seam_bottom2);
    glVertex3i (dest_seam_left, dest_seam_bottom, 0);
    glTexCoord2i (common->src_seam_right1, common->src_seam_bottom2);
    glVertex3i (dest_seam_right + 1, dest_seam_bottom, 0);
    glTexCoord2i (common->src_seam_right1, common->texture_height);
    glVertex3i (dest_seam_right + 1, 0, 0);

    // Bottom-right slice.
    glTexCoord2i (common->src_seam_right2, common->texture_height);
    glVertex3i (dest_seam_right, 0, 0);
    glTexCoord2i (common->src_seam_right2, common->src_seam_bottom2);
    glVertex3i (dest_seam_right, dest_seam_bottom, 0);
    glTexCoord2i (common->texture_width, common->src_seam_bottom2);
    glVertex3i (width, dest_seam_bottom, 0);
    glTexCoord2i (common->texture_width, common->texture_height);
    glVertex3i (width, 0, 0);

    // Middle-left slice.
    glTexCoord2i (0, common->src_seam_bottom1);
    glVertex3i (0, dest_seam_bottom - 1, 0);
    glTexCoord2i (0, common->src_seam_top2);
    glVertex3i (0, dest_seam_top, 0);
    glTexCoord2i (common->src_seam_left1, common->src_seam_top2);
    glVertex3i (dest_seam_left, dest_seam_top, 0);
    glTexCoord2i (common->src_seam_left1, common->src_seam_bottom1);
    glVertex3i (dest_seam_left, dest_seam_bottom - 1, 0);

    // Middle slice.
    glTexCoord2i (common->src_seam_left2, common->src_seam_bottom1);
    glVertex3i (dest_seam_left, dest_seam_bottom - 1, 0);
    glTexCoord2i (common->src_seam_left2, common->src_seam_top2);
    glVertex3i (dest_seam_left, dest_seam_top, 0);
    glTexCoord2i (common->src_seam_right1, common->src_seam_top2);
    glVertex3i (dest_seam_right + 1, dest_seam_top, 0);
    glTexCoord2i (common->src_seam_right1, common->src_seam_bottom1);
    glVertex3i (dest_seam_right + 1, dest_seam_bottom - 1, 0);

    // Middle-right slice.
    glTexCoord2i (common->src_seam_right2, common->src_seam_bottom1);
    glVertex3i (dest_seam_right, dest_seam_bottom - 1, 0);
    glTexCoord2i (common->src_seam_right2, common->src_seam_top2);
    glVertex3i (dest_seam_right, dest_seam_top, 0);
    glTexCoord2i (common->texture_width, common->src_seam_top2);
    glVertex3i (width, dest_seam_top, 0);
    glTexCoord2i (common->texture_width, common->src_seam_bottom1);
    glVertex3i (width, dest_seam_bottom - 1, 0);

    // Top-left slice.
    glTexCoord2i (0, common->src_seam_top1);
    glVertex3i (0, dest_seam_top, 0);
    glTexCoord2i (0, 0);
    glVertex3i (0, height, 0);
    glTexCoord2i (common->src_seam_left1, 0);
    glVertex3i (dest_seam_left, height, 0);
    glTexCoord2i (common->src_seam_left1, common->src_seam_top1);
    glVertex3i (dest_seam_left, dest_seam_top, 0);

    // Top-middle slice.
    glTexCoord2i (common->src_seam_left2, common->src_seam_top1);
    glVertex3i (dest_seam_left, dest_seam_top, 0);
    glTexCoord2i (common->src_seam_left2, 0);
    glVertex3i (dest_seam_left, height, 0);
    glTexCoord2i (common->src_seam_right1, 0);
    glVertex3i (dest_seam_right + 1, height, 0);
    glTexCoord2i (common->src_seam_right1, common->src_seam_top1);
    glVertex3i (dest_seam_right + 1, dest_seam_top, 0);

    // Top-right slice.
    glTexCoord2i (common->src_seam_right2, common->src_seam_top1);
    glVertex3i (dest_seam_right, dest_seam_top, 0);
    glTexCoord2i (common->src_seam_right2, 0);
    glVertex3i (dest_seam_right, height, 0);
    glTexCoord2i (common->texture_width, 0);
    glVertex3i (width, height, 0);
    glTexCoord2i (common->texture_width, common->src_seam_top1);
    glVertex3i (width, dest_seam_top, 0);
  glEnd();

  glDisable (GL_TEXTURE_2D);

  glLoadIdentity(); // texture matrix

  glPopAttrib(); // restore the previous matrix mode
}
