/*
 * NineSlice.h
 *
 *  Created on: 2011-12-21
 *      Author: samuel
 */

#ifndef NINESLICE_H
#define NINESLICE_H

#include <string>
#include <GL/gl.h>
#include "Rectangle.h"

namespace Game {
  class NineSlice {
  private:
    // Common data shared between nine slices that use the same texture.
    struct Common {
      GLuint texture;
      int texture_width, texture_height; // in pixels
      Rectangle texture_middle; // in pixels from top-left of texture
      static const int PADDING = 1; // pixels between slices in texture

      // Source seams originate from the top-left of the texture.
      GLint src_seam_left1;
      GLint src_seam_left2;
      GLint src_seam_right1;
      GLint src_seam_right2;
      GLint src_seam_top1;
      GLint src_seam_top2;
      GLint src_seam_bottom1;
      GLint src_seam_bottom2;

      int reference_count;
    } *common;

    int width, height; // in pixels

    // Destination seams originate from the bottom-left of the destination
    // rectangle.
    GLint dest_seam_left;
    GLint dest_seam_right;
    GLint dest_seam_top;
    GLint dest_seam_bottom;

  public:
    NineSlice() throw();
    NineSlice (const NineSlice &nine_slice) throw();
    virtual ~NineSlice() throw();

    // Sets the size in pixels.
    void set_size (int width, int height) throw();

    // Draws this nine slice with the origin at the bottom-left.
    void draw() const throw();
  };
} // namespace Game

#endif // NINESLICE_H
