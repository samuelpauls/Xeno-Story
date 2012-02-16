/*
 * PathFollower.h
 *
 *  Created on: 2012-02-08
 *      Author: samuel
 */

#ifndef PATHFOLLOWER_H
#define PATHFOLLOWER_H

#include "Path.h"

namespace Game {
  namespace SpriteLayer {
    // Follows a path.
    class PathFollower {
    private:
      Path *path; // not owned by this path follower

      // Using an iterator would be independent of the collection
      // implementation.  However duplicating the path requires duplicating the
      // iterator so that it points to an element in the new path, which can't
      // be done.
      int current_node_index, next_node_index;

      // Decrement or increment an iterator and wrap at the end.
      int get_previous_node_index (int index) throw();
      int get_next_node_index (int index) throw();

      // Information regarding the starting position, which is usually the
      // current node.  It could also be an arbitrary position.  The main thing
      // is that it's a start point on the way to the next node.
      unsigned int start_tick; // ms at start of current node
      int start_x, start_y;

      int x, y; // in pixels

      // Get the milliseconds until the next node.
      unsigned int get_ms_to_next() const throw();

    public:
      PathFollower() throw();

      PathFollower (const PathFollower &path_follower) throw();

      virtual ~PathFollower() throw();

      // Sets the position of this path follower.  From here it will proceed to
      // the next node at the same speed as if the given position were the
      // prior node.
      void set_position (int x, int y) throw();

      // Remember to manually delete the path when done with it.
      void set_path (Path *path) throw();

      // tick is ms since program start
      void update (unsigned int tick) throw();

      int get_x() const throw() { return x; }
      int get_y() const throw() { return y; }
    };
  } // namespace SpriteLayer
} // namespace Game

#endif // PATHFOLLOWER_H
