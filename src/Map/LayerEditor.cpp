/*
 * LayerEditor.cpp
 *
 *  Created on: 2011-09-02
 *      Author: samuel
 */

#include "../Window.h"
#include "../UI/Frame.h"
#include "Editor.h"
#include "LayerEditor.h"

using Game::Map::LayerEditor;

LayerEditor::LayerEditor (UI::Manager &ui_manager) throw():
    ui_manager (ui_manager) {
  palette_frame_rectangle.x = 0;
  palette_frame_rectangle.w = Window::get_width();
  palette_frame_rectangle.h = UI::Frame::TOP_PADDING + Editor::ITEM_PADDING + 50
      + Editor::ITEM_PADDING + UI::Frame::PADDING;
  palette_frame_rectangle.y = Window::get_height() - palette_frame_rectangle.h;
}

LayerEditor::~LayerEditor() throw() {}
