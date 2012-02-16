/*
 * Lighting.h
 *
 *  Created on: 2011-09-07
 *      Author: samuel
 */

#ifndef Lighting_H
#define Lighting_H

#include <list>
#include <GL/gl.h>

// "glext.h" isn't available when compiling for Windows, so a copy is included
// in the source directory.
#include "glext.h"

#include "Map/Layer.h"

namespace Game {
  namespace TileLayer {
    class TileLayout;
  }

  // Performs lighting calculations based on each visible tile's "dark" property.
  class Lighting: public Map::Layer {
  private:
    PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
    PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
    PFNGLUNIFORM1IARBPROC glUniform1iARB;
    PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
#ifdef WINDOWS
    PFNGLACTIVETEXTUREPROC glActiveTexture;
    PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i;
    PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
#endif

    GLenum fragment_shader;
    GLhandleARB program;

    GLuint framebuffer;

    // Each tile has a corresponding pixel on the light map.  Thus the light map
    // is only a twenty-fifth of the viewport size.  Each pixel's red value on the
    // light map specifies the brightness of it's corresponding tile.
    //sf::Image light_map;
    GLuint light_map;

    // The layer of tiles to check for the "dark" property.
    const TileLayer::TileLayout &tile_layer;

    int visible_tiles_high, visible_tiles_wide; // for viewport, no scrolling

    static const int MAX_LIGHTS = 20;
    GLint light_count;
    GLuint lights_data;

    bool shader_compiled;
    // Returns true only if post processing is supported.
    bool is_supported() const throw();

  public:
    Lighting (const TileLayer::TileLayout &tile_layer) throw();
    virtual ~Lighting() throw();

    // radius is in pixels.
    // The color and location come from OpenGL's color and origin.
    void add_light (int radius) throw();

    virtual void draw (Lighting &lighting, const Rectangle &viewport) throw();
  };
}

#endif // Lighting_H
