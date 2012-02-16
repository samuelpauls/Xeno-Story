Xeno Story
==========

Welcome to a beta release of Xeno Story.
  
When you leave the level editor, the tile layers are automatically saved.
Presently, the only way to add additional maps is to place a door sprite within
an existing map that the player can reach and then manually creating and editing
the map files in a way that follows the existing map file convention.  (Yes this
will be improved.)

The game engine features:
- Multiple layers.  A typical area has a background image, several layers of
  tiles, sprites, and lighting layers.
- Customisable.  Nearly everything is loaded from configuration and image files
  as opposed to being hard coded.  The level editor which is nearing a usable
  state will assist.
- Cross platform.  The engine is designed for easy porting and already compiles
  for Windows and Linux.
- Speed.  The game is designed using C++ and (indirectly) OpenGL.  Lighting is
  emberassingly parallel with pixel shaders.  Further optimisations will assist
  in fulfilling this objective.
  
Known issues:
- The level editor can only be used to edit certain aspects of the level.  For
  now, the remaining aspects must be manually edited in the filesystem.