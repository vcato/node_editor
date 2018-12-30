#include "fakediagrameditor.hpp"

#include <iostream>

using std::cerr;

static int character_width = 10;

ViewportRect
  FakeDiagramEditor::rectAroundText(
    const ViewportTextObject &text_object
  ) const
{
  assert(text_object.text.find('\n')==text_object.text.npos);
    // Don't have the logic for handling multi-line text objects yet.

  // Just assume all characters are fixed size for now.
  ViewportCoords begin_pos =
    viewportCoordsFromDiagramCoords(text_object.position);
  ViewportCoords end_pos = begin_pos;
  end_pos.x += text_object.text.length() * character_width;
  end_pos.y += characterHeight();
  return {begin_pos,end_pos};
}


ViewportLine
  FakeDiagramEditor::textObjectCursorLine(
    const ViewportTextObject &text_object,
    int column_index
  ) const
{
  ViewportCoords bottom =
    text_object.position + Vector2D(character_width*column_index,0);
  ViewportCoords top = bottom + Vector2D(0,characterHeight());
  return ViewportLine{bottom,top};
}
