/*
 * Lighting.cpp
 *
 *  Created on: 2011-09-07
 *      Author: samuel
 */

#include <iostream>
#include <cassert>
#include <fstream>
#ifdef WINDOWS
  #define GL_GLEXT_PROTOTYPES
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Lighting.h"
#ifdef UNIX
  #include <GL/glx.h>
#endif
#include "UserSettings.h"
#include "Window.h"

using Game::Lighting;

bool Lighting::is_supported() const throw() {
  return glewIsSupported ("GL_ARB_shading_language_100") &&
         glewIsSupported ("GL_ARB_shader_objects") &&
         glewIsSupported ("GL_ARB_fragment_shader") &&
         shader_compiled;
}

Lighting::Lighting (const TileLayer::TileLayout &tile_layer) throw():
                   Layer ("Lighting"), tile_layer (tile_layer) {
  shader_compiled = true; // set to true initially so is_supported() has chance
  if (!is_supported()) {
    return;
  }

  #ifdef UNIX
    PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB =
        (PFNGLCREATESHADEROBJECTARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glCreateShaderObjectARB");
    PFNGLSHADERSOURCEARBPROC glShaderSourceARB =
        (PFNGLSHADERSOURCEARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glShaderSourceARB");
    PFNGLCOMPILESHADERARBPROC glCompileShaderARB =
        (PFNGLCOMPILESHADERARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glCompileShaderARB");
    PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB =
        (PFNGLCREATEPROGRAMOBJECTARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glCreateProgramObjectARB");
    PFNGLATTACHOBJECTARBPROC glAttachObjectARB =
        (PFNGLATTACHOBJECTARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glAttachObjectARB");
    PFNGLLINKPROGRAMARBPROC glLinkProgramARB =
        (PFNGLLINKPROGRAMARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glLinkProgramARB");
    PFNGLGETINFOLOGARBPROC glGetInfoLogARB =
        (PFNGLGETINFOLOGARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glGetInfoLogARB");
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog =
        (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress (
            (const GLubyte*)"glGetProgramInfoLog");
    PFNGLGETSHADERIVPROC glGetShaderiv =
        (PFNGLGETSHADERIVPROC)glXGetProcAddress (
            (const GLubyte*)"glGetShaderiv");
    PFNGLGETPROGRAMIVPROC glGetProgramiv =
        (PFNGLGETPROGRAMIVPROC)glXGetProcAddress (
            (const GLubyte*)"glGetProgramiv");

    glUseProgramObjectARB =
        (PFNGLUSEPROGRAMOBJECTARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glUseProgramObjectARB");
    glGetUniformLocationARB =
        (PFNGLGETUNIFORMLOCATIONARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glGetUniformLocationARB");
    glUniform1iARB =
        (PFNGLUNIFORM1IARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glUniform1iARB");
    glDeleteObjectARB =
        (PFNGLDELETEOBJECTARBPROC)glXGetProcAddressARB (
            (const GLubyte*)"glDeleteObjectARB");
  #endif

  #ifdef WINDOWS
    PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB =
        (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress (
            "glCreateShaderObjectARB");
    PFNGLSHADERSOURCEARBPROC glShaderSourceARB =
        (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress ("glShaderSourceARB");
    PFNGLCOMPILESHADERARBPROC glCompileShaderARB =
        (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress ("glCompileShaderARB");
    PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB =
        (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress (
            "glCreateProgramObjectARB");
    PFNGLATTACHOBJECTARBPROC glAttachObjectARB =
        (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress ("glAttachObjectARB");
    PFNGLLINKPROGRAMARBPROC glLinkProgramARB =
        (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress ("glLinkProgramARB");
    PFNGLGETINFOLOGARBPROC glGetInfoLogARB =
        (PFNGLGETINFOLOGARBPROC)wglGetProcAddress ("glGetInfoLogARB");
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog =
        (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress ("glGetProgramInfoLog");
    PFNGLGETSHADERIVPROC glGetShaderiv =
        (PFNGLGETSHADERIVPROC)wglGetProcAddress ("glGetShaderiv");
    PFNGLGETPROGRAMIVPROC glGetProgramiv =
        (PFNGLGETPROGRAMIVPROC)wglGetProcAddress ("glGetProgramiv");

    glUseProgramObjectARB =
        (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress ("glUseProgramObjectARB");
    glGetUniformLocationARB =
        (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress (
            "glGetUniformLocationARB");
    glUniform1iARB =
        (PFNGLUNIFORM1IARBPROC)wglGetProcAddress ("glUniform1iARB");
    glActiveTexture =
        (PFNGLACTIVETEXTUREPROC)wglGetProcAddress ("glActiveTextureARB");
    glMultiTexCoord2i =
        (PFNGLMULTITEXCOORD2IPROC)wglGetProcAddress ("glMultiTexCoord2i");
    glMultiTexCoord2f =
        (PFNGLMULTITEXCOORD2FPROC)wglGetProcAddress ("glMultiTexCoord2f");
    glDeleteObjectARB =
        (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress ("glDeleteObjectARB");
  #endif

  // If wglGetProcAddress or glXGetProcAddressARB returns NULL, the requested
  // function failed to be retrieved.
  if (glCreateShaderObjectARB == NULL ||
      glShaderSourceARB == NULL ||
      glCompileShaderARB == NULL ||
      glCreateProgramObjectARB == NULL ||
      glAttachObjectARB == NULL ||
      glLinkProgramARB == NULL ||
      glGetInfoLogARB == NULL ||
      glGetProgramInfoLog == NULL ||
      glGetShaderiv == NULL ||
      glGetProgramiv == NULL ||
      glUseProgramObjectARB == NULL ||
      glGetUniformLocationARB == NULL ||
      glUniform1iARB == NULL ||
      glDeleteObjectARB == NULL) {
    shader_compiled = false;
    return;
  }

  fragment_shader = glCreateShaderObjectARB (GL_FRAGMENT_SHADER_ARB);

  // Load the shader program that will be executed in parallel for each pixel
  // on the GPU.
  std::string frag_shader_src;
  std::ifstream f ("lighting.glsl");
  assert (f);
  while (!f.eof()) {
    std::string line;
    std::getline (f, line);
    frag_shader_src += line;
  }
  f.close();
  const char *c_src = frag_shader_src.c_str();
  glShaderSourceARB (fragment_shader, 1, &c_src, NULL);

  static const int MAX_LOG_CHARS = 1000;

  glCompileShaderARB (fragment_shader);
  GLint compiled;
  glGetShaderiv (fragment_shader, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE) {
    char log[MAX_LOG_CHARS];
    glGetInfoLogARB (fragment_shader, MAX_LOG_CHARS, NULL, log);
    std::cout << "Lighting shader compile log:\n" << log << std::endl;
    glDeleteObjectARB (fragment_shader);
    shader_compiled = false;
    return;
  }

  program = glCreateProgramObjectARB();

  glAttachObjectARB (program, fragment_shader);

  glLinkProgramARB (program);
  GLint linked;
  glGetProgramiv (program, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE) {
    char log[MAX_LOG_CHARS];
    glGetProgramInfoLog (program, MAX_LOG_CHARS, NULL, log);
    std::cout << "Lighting shader link log:\n" << log << std::endl;
    glDeleteObjectARB (fragment_shader);
    glDeleteObjectARB (program);
    shader_compiled = false;
    return;
  }

  glGenTextures (1, &framebuffer);
  glBindTexture (GL_TEXTURE_2D, framebuffer);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, Window::get_width(), Window::get_height(), 0, GL_RGBA, GL_FLOAT, NULL);

  // Set properties of the bound texture.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Calculate the number of tiles that can fit in the viewport without
  // scrolling.
  visible_tiles_wide = Window::get_width()  / tile_layer.get_tile_size();
  visible_tiles_high = Window::get_height() / tile_layer.get_tile_size();

  // Create a "light map" which is really a texture where each pixel's red
  // value represents the color of a corresponding tile.  An extra column or row
  // of tiles may be required on one end of the screen because those on the
  // other end may be partially off the screen.
  glGenTextures (1, &light_map);
  glBindTexture (GL_TEXTURE_2D, light_map);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, visible_tiles_wide + 1, visible_tiles_high + 1, 0, GL_RGBA, GL_FLOAT, NULL);

  // Smooth transitions between light tiles.
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Don't blend in undefined light map pixels from past the edge of the
  // texture.
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures (1, &lights_data);
  glBindTexture (GL_TEXTURE_2D, lights_data);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, MAX_LIGHTS, 2, 0, GL_RGB, GL_FLOAT, NULL);

  // Ensure the texels making up the light data don't corrupt by mixing.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  light_count = 0;
}

Lighting::~Lighting() throw() {
  if (!is_supported()) {
    return;
  }

  glDeleteTextures (1, &framebuffer);
  glDeleteTextures (1, &light_map);
  glDeleteTextures (1, &lights_data);

  glDeleteObjectARB (fragment_shader);
  glDeleteObjectARB (program);
}

void Lighting::add_light (int radius) throw() {
  if (!is_supported()) {
    return;
  }

  if (light_count < MAX_LIGHTS) {
    glBindTexture (GL_TEXTURE_2D, lights_data);

    GLdouble gl_origin_x, gl_origin_y, gl_one_x, gl_scale, dummy; // in pixels

    // Get OpenGL's drawing origin as the light's location.
    GLdouble modelview[16], projection[16];
    GLint viewport[4];
    glGetDoublev (GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev (GL_PROJECTION_MATRIX, projection);
    glGetIntegerv (GL_VIEWPORT, viewport);
    gluProject (0.0, 0.0, 0.0, // object
                modelview,
                projection,
                viewport,
                &gl_origin_x, &gl_origin_y, &dummy);

    // Get OpenGL's modelview scale as the light's scale.
    gluProject (1.0, 0.0, 0.0, // object
                modelview,
                projection,
                viewport,
                &gl_one_x, &dummy, &dummy);
    gl_scale = gl_one_x - gl_origin_x;

    // Encode the location into the light data texture.
    static const float PADDING = 100.0F;
    GLfloat pixel[3];
    pixel[0] = (gl_origin_x + PADDING) / (PADDING + Window::get_width() + PADDING);
    pixel[1] = (gl_origin_y + PADDING) / (PADDING + Window::get_height() + PADDING);
    pixel[2] = (float)(gl_scale * radius) / Window::get_width();
    glTexSubImage2D (GL_TEXTURE_2D, 0, light_count,
                     0, 1, 1, GL_RGB, GL_FLOAT, pixel);

    // Get OpenGL's color as the light color and encode it into the light data
    // texture.
    GLfloat gl_color[4];
    glGetFloatv (GL_CURRENT_COLOR, gl_color);
    glTexSubImage2D (GL_TEXTURE_2D, 0, light_count, 1,
                     1, 1, GL_RGB, GL_FLOAT, gl_color);

    ++light_count;
  }
}

void Lighting::draw (Lighting &lighting, const Rectangle &viewport) throw() {
  if (!is_supported() || !UserSettings::is_lighting_enabled()) {
    return;
  }

  glPushMatrix(); // save the position before applying lighting

  glBindTexture (GL_TEXTURE_2D, framebuffer);
  glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 0, 0, Window::get_width(), Window::get_height());

  // Go through each viewport tile, starting at the top-left.
  glBindTexture (GL_TEXTURE_2D, light_map);
  for (int y = 0; y < visible_tiles_high + 1; ++y) {
    for (int x = 0; x < visible_tiles_wide + 1; ++x) {
      // Get the x and y map tile index for this viewport tile.
      int tile_x = viewport.x / tile_layer.get_tile_size() + x;
      if (viewport.x < 0 && -viewport.x % tile_layer.get_tile_size() != 0) {
        --tile_x;
      }
      int tile_y = viewport.y / tile_layer.get_tile_size() + y;
      if (viewport.y <= 0 || viewport.y % tile_layer.get_tile_size() == 0) {
        --tile_y;
      }

      static const GLfloat BRIGHT_PIXEL[] = {1.0F, 1.0F, 1.0F, 1.0F};
      static const GLfloat DARK_PIXEL[] = {0.5F, 0.5F, 0.5F, 1.0F};
      const GLfloat *pixel = BRIGHT_PIXEL;

      // Calculate the amount of light for this particular map tile.  (If the
      // tile index represents a tile outside of the map, be bright.)
      if (tile_x >= 0 &&
          tile_y >= 0 &&
          tile_x < tile_layer.get_tiles_wide() &&
          tile_y < tile_layer.get_tiles_high()) {
        TileLayer::Tile *tile = tile_layer.get_tile (tile_x, tile_y);
        if (tile && tile->is_dark()) {
          pixel = DARK_PIXEL;
        }
      }

      glTexSubImage2D (GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);
    }
  }

  // Must use the fragement shading program before setting its values.
  glUseProgramObjectARB (program);

  int light_count_location = glGetUniformLocationARB (program, "light_count");
  assert (light_count_location != -1);
  glUniform1iARB (light_count_location, light_count);

  GLint lights_data_loc = glGetUniformLocationARB (program, "lights_data");
  assert (lights_data_loc != -1);
  glActiveTexture (GL_TEXTURE0 + 2);
  glBindTexture (GL_TEXTURE_2D, lights_data);
  glUniform1iARB (lights_data_loc, 2);

  // Set the offset into the top left light tile to be drawn.
  int pixel_offset_x = viewport.x % tile_layer.get_tile_size();
  while (pixel_offset_x < 0) {
    pixel_offset_x += tile_layer.get_tile_size();
  }
  int pixel_offset_y = -viewport.y % tile_layer.get_tile_size();
  while (pixel_offset_y < 0) {
    pixel_offset_y += tile_layer.get_tile_size();
  }

  int light_map_width = (visible_tiles_wide + 1) * tile_layer.get_tile_size();
  int light_map_height = (visible_tiles_high + 1) * tile_layer.get_tile_size();
  float light_map_xl = (float)pixel_offset_x / light_map_width;
  float light_map_xr = light_map_xl + (float)viewport.w / light_map_width;
  float light_map_yt = 1.0F - (float)pixel_offset_y / light_map_height;
  float light_map_yb = light_map_yt - (float)viewport.h / light_map_height;

  GLint light_map_location = glGetUniformLocationARB (program, "light_map");
  assert (light_map_location != -1);
  glActiveTexture (GL_TEXTURE0 + 1);
  glBindTexture (GL_TEXTURE_2D, light_map);
  glUniform1iARB (light_map_location, 1);

  GLint framebuffer_location = glGetUniformLocationARB (program, "framebuffer");
  assert (framebuffer_location != -1);
  glActiveTexture (GL_TEXTURE0 + 0);
  glBindTexture (GL_TEXTURE_2D, framebuffer);
  glUniform1iARB (framebuffer_location, 0);

  glMatrixMode (GL_TEXTURE);
  glLoadIdentity();

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  glBegin (GL_QUADS);
    glMultiTexCoord2i (GL_TEXTURE0, 0, 0); // framebuffer
    glMultiTexCoord2f (GL_TEXTURE1, light_map_xl, light_map_yb); // light map
    glMultiTexCoord2i (GL_TEXTURE2, 0, 0); // window pixels
    glVertex2i (0, 0);

    glMultiTexCoord2i (GL_TEXTURE0, 0, 1);
    glMultiTexCoord2f (GL_TEXTURE1, light_map_xl, light_map_yt);
    glMultiTexCoord2i (GL_TEXTURE2, 0, Window::get_height());
    glVertex2i (0, Window::get_height());

    glMultiTexCoord2i (GL_TEXTURE0, 1, 1);
    glMultiTexCoord2f (GL_TEXTURE1, light_map_xr, light_map_yt);
    glMultiTexCoord2i (GL_TEXTURE2, Window::get_width(), Window::get_height());
    glVertex2i (Window::get_width(), Window::get_height());

    glMultiTexCoord2i (GL_TEXTURE0, 1, 0);
    glMultiTexCoord2f (GL_TEXTURE1, light_map_xr, light_map_yb);
    glMultiTexCoord2i (GL_TEXTURE2, Window::get_width(), 0);
    glVertex2i (Window::get_width(), 0);
  glEnd();

  glUseProgramObjectARB (0);

  light_count = 0;

  glPopMatrix(); // restore the position to that of before drawing lighting
}
