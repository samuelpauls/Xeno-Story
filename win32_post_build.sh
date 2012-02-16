# win32_post_build.sh
#
# Created on: 2011-09-25
# Author: samuel

# This script creates a Windows package from a binary, the game's resource
# files, and a few third party libraries.

# This script uses "Windows Debug" or "Windows Release" as the current directory.

# -p for parent (only create parents that don't exist)
mkdir -p Xeno\ Story

cp -r Xeno\ Story.exe \
  ../avatar_status \
  ../achievements \
  ../credits.dat \
  ../gate \
  ../image_layers \
  ../lighting.glsl \
  ../maps \
  ../music \
  ../message_box \
  ../pickup.ogg \
  ../Readme.txt \
  ../sprites \
  ../start.dat \
  ../switch_weapon.ogg \
  ../tiles \
  ../title_flash.ogg \
  ../ui \
  ../user_settings.dat \
  ../win32_libraries/* \
  Xeno\ Story

# -r for recursing into directories
zip -r Xeno\ Story.zip Xeno\ Story
