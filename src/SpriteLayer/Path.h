/*
 * Path.h
 *
 *  Created on: 2012-02-04
 *      Author: samuel
 */

#ifndef PATH_H
#define PATH_H

#include <vector>

namespace Game {
  namespace SpriteLayer {
    // Provides the current location along a path of waypoints.
    class Path {
      friend class PathFollower;

    private:
      struct Node {
        int x, y; // in pixels
        unsigned int speed_per_second; // speed is in pixels
      };
      std::vector<Node*> nodes;

    public:
      Path();
      Path (const Path &path) throw();
      virtual ~Path();

      // speed_per_second is in pixels
      void push_node (int x, int y, unsigned int speed_per_second) throw();

      // speed_per_second is in pixels
      void insert_node (int index,
                        int x, int y,
                        unsigned int speed_per_second) throw();

      void remove_node (int index) throw();
    }; // class Path
  } // namespace SpriteLayer
} // namespace Game

#endif // PATH_H
