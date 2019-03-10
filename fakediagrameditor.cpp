#include "fakediagrameditor.hpp"

#include <iostream>

using std::cerr;

static int character_width = 10;


ViewportRect FakeDiagramEditor::rectAroundText(const std::string &text) const
{
  assert(text.find('\n') == text.npos);
    // Don't have the logic for handling multi-line text objects yet.

  ViewportCoords begin_pos = {0,0};
  ViewportCoords end_pos = begin_pos;

  // Just assume all characters are fixed size for now.
  end_pos.x += text.length() * character_width;
  end_pos.y += characterHeight();

  return ViewportRect{begin_pos,end_pos};
}


ViewportLine
  FakeDiagramEditor::textObjectCursorLine(
    const ViewportTextObject &text_object,
    int column_index
  ) const
{
  ViewportCoords bottom =
    text_object.position + ViewportVector(character_width*column_index,0);
  ViewportCoords top = bottom + ViewportVector(0,characterHeight());
  return ViewportLine{bottom,top};
}
