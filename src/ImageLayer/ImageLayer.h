/*
 * ImageLayer.h
 *
 *  Created on: 2011-08-23
 *      Author: samuel
 */

#ifndef IMAGELAYER_H
#define IMAGELAYER_H

#include <string>
#include "../Rectangle.h"
#include "../Animation.h"
#include "../World.h"
#include "../Map/Layer.h"

namespace Game {
  class Lighting;
  namespace ImageLayer {
    // A world layer that draws a stretched animation across the viewport.
    class ImageLayer: public Map::Layer {
    private:
      std::string data_filename;
      std::string animation_filename; // a filename inside of "./image_layers/"
      Animation *animation;

    public:
      ImageLayer (const std::string &map_path,
                  const std::string &data_filename) throw();
      virtual ~ImageLayer() throw();

      // Processes a command dealing with this image layer.
      // Returns true if the command was executed or false if not.
      bool process_command (const std::string &command) throw();

      // Change the image displayed by this layer.
      // Filename is the filename of the animation inside of "./image_layers".
      void set_animation (Animation *animation,
                          const std::string &filename) throw();

      std::string get_animation_filename() const throw() {
        return animation_filename;
      }

      void update (World &world, unsigned int tick) throw() {
        animation->update (tick);
      }
      void draw (Lighting &lighting, const Rectangle &viewport) throw();

      // Saves this image layer to disk.
      virtual void save (const std::string &map_path) throw();
    }; // class ImageLayer
  } // namespace ImageLayer
} // namespace Game

#endif // IMAGELAYER_H
