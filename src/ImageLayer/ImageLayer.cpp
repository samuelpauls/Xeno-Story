/*
 * ImageLayer.cpp
 *
 *  Created on: 2011-08-23
 *      Author: samuel
 */

#include <fstream>
#include "../StringTokenizer.h"
#include "ImageLayer.h"

using Game::ImageLayer::ImageLayer;

ImageLayer::ImageLayer (const std::string &map_path,
                        const std::string &data_filename) throw():
    Map::Layer ("Background Image") {
  this->data_filename = data_filename;
  animation = NULL;

  // Load the image from the layer's configuration file.
  std::string filename = map_path + '/' + data_filename;
  std::ifstream f (filename.c_str());
  std::string image_filename;
  if (f) {
    std::getline (f, image_filename);
    f.close();
  } else {
    image_filename = "clouds.png";
  }
  set_animation (new Animation ("image_layers/" + image_filename),
                 image_filename);
}

void ImageLayer::set_animation (Animation *animation,
                                const std::string &filename) throw() {
  delete this->animation;

  this->animation_filename = filename;
  this->animation = animation;
}

ImageLayer::~ImageLayer() throw() {
  delete animation;
}

bool ImageLayer::process_command (const std::string &command) throw() {
  if (!command.empty()) {
    StringTokenizer tokens (command);
    std::string action = tokens.get_next_string();
    if (action == "set_background_image") {
      std::string filename = tokens.get_next_string();
      set_animation (new Animation ("image_layers/" + filename), filename);
      return true;
    } // if a particular action
  }
  return false;
}

void ImageLayer::draw (Lighting &lighting, const Rectangle &viewport) throw() {
  glPushMatrix();
  glLoadIdentity();
  GLfloat scale = (GLfloat)Window::get_width() / animation->get_width();
  glScalef (scale, scale, 1.0F);
  animation->draw();
  glPopMatrix();
}

void ImageLayer::save (const std::string &map_path) throw() {
  std::string filename = map_path + '/' + data_filename;
  std::ofstream f (filename.c_str());
  f << animation_filename;
  f.close();
}
