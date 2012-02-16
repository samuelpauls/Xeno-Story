/*
 * ImageLayerWidget.h
 *
 *  Created on: 2012-01-01
 *      Author: samuel
 */

#ifndef IMAGELAYERWIDGET_H
#define IMAGELAYERWIDGET_H

#include <string>
#include "../Animation.h"
#include "../UI/MenuItem.h"

namespace Game {
  namespace ImageLayer {
    class ImageLayerWidget: public UI::MenuItem {
    private:
      std::string filename;
      Animation *animation;

    public:
      // The animation is automatically deleted when the image layer widget is
      // deleted.
      ImageLayerWidget (Animation *animation,
                        const std::string &filename) throw();
      virtual ~ImageLayerWidget() throw();

      std::string get_filename() const throw() { return filename; }
      Animation *get_animation() throw() { return animation; }

      virtual void draw() const throw();
    }; // class ImageLayerWidget
  } // namespace ImageLayer
} // namespace Game

#endif // IMAGELAYERWIDGET_H
