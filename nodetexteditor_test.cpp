#include "nodetexteditor.hpp"

#include <cassert>
#include <sstream>


using std::vector;
using std::string;
using std::istringstream;
using std::ostringstream;


namespace {
struct Tester {
  struct Callbacks : NodeTextEditor::Callbacks {
    ostringstream stream;

    virtual void lineUnfocused(int line_index)
    {
      stream << "lineUnfocused(" << line_index << ")\n";
    }


    virtual void lineFocused(int line_index)
    {
      stream << "lineFocused(" << line_index << ")\n";
    }
  };

  Callbacks callbacks;
  NodeTextEditor editor;
  Node node;

  Tester()
  : editor()
  {
  }


  void testDownWithNoText()
  {
    editor.beginEditing(node);
    editor.down();
  }

  void beginEditing(const char *text)
  {
    node.setText(text);
    editor.beginEditing(node);
  }

  void testBeginEditing()
  {
    beginEditing("a");
    assert(editor.cursorLineIndex()==0);
    assert(editor.cursorColumnIndex()==1);
  }

  void testLeft1()
  {
    beginEditing("aa");
    editor.moveCursor(/*new_line_index*/0,/*new_column_index*/2);
    editor.left();
    assert(editor.cursorColumnIndex()==1);
    editor.left();
    assert(editor.cursorColumnIndex()==0);
    editor.left();
    assert(editor.cursorColumnIndex()==0);
  }

  void testLeft2()
  {
    beginEditing("a\naa");
    editor.moveCursor(/*new_line_index*/1,/*new_column_index*/2);
    editor.up();
    editor.left();
    assert(editor.cursorColumnIndex()==0);
  }

  void testRight()
  {
    beginEditing("aa");
    editor.moveCursor(/*new_line_index*/0,/*new_column_index*/0);
    editor.right();
    assert(editor.cursorColumnIndex()==1);
    editor.right();
    assert(editor.cursorColumnIndex()==2);
    editor.right();
    assert(editor.cursorColumnIndex()==2);
  }

  void testUp()
  {
    beginEditing("a\nab");
    editor.moveCursor(/*line*/1,/*column*/2);
    clearOutput();
    editor.up(callbacks);
    assert(output()=="lineUnfocused(1)\nlineFocused(0)\n");
    editor.textTyped("c");
  }

  void testBackspace1()
  {
    beginEditing("aa");
    editor.moveCursor(/*line*/0,/*column*/2);
    editor.backspace();
    assert(node.lines[0].text=="a");
    assert(editor.cursorColumnIndex()==1);
    editor.backspace();
    assert(node.lines[0].text=="");
    assert(editor.cursorColumnIndex()==0);
    editor.backspace();
  }

  void testBackspace2()
  {
    beginEditing("ab");
    editor.moveCursor(/*line*/0,/*column*/1);
    editor.backspace();
    assert(node.lines[0].text=="b");
    editor.backspace();
  }

  void testBackspace3()
  {
    beginEditing("a\n");
    assert(node.lines.size()==2);
    editor.moveCursor(/*line*/1,/*column*/0);
    editor.backspace();
    assert(editor.cursorColumnIndex()==1);
    assert(editor.cursorLineIndex()==0);
  }

  void testBackspace4()
  {
    beginEditing("a = $\n$ = a");
    editor.moveCursor(/*line*/1,/*column*/0);
    editor.backspace();
    assert(node.lines.size()==1);
    assert(!lineTextHasOutput(node.lines[0].text));
    assert(node.nOutputs()==0);
  }

  void testBackspace5()
  {
    beginEditing("a\naa");
    editor.moveCursor(/*line*/1,/*column*/2);
    editor.up();
    editor.backspace();
  }

  void testDown()
  {
    beginEditing("a\nb");
    assert(node.lines.size()==2);
    editor.moveCursorToLine(0);
    editor.down();
    assert(editor.cursorLineIndex()==1);
    editor.down();
    assert(editor.cursorLineIndex()==1);
  }


  void testTextTyped1()
  {
    beginEditing("");
    editor.textTyped("ab");
    assert(node.lines[0].text=="ab");
  }

  void testTextTyped2()
  {
    beginEditing("ab");
    editor.moveCursorToColumn(1);
    editor.textTyped("=");
    assert(node.lines[0].text=="a=b");
    assert(editor.cursorColumnIndex()==2);
  }

  void clearOutput()
  {
    callbacks.stream.str("");
  }

  string output()
  {
    return callbacks.stream.str();
  }

  void testEnter1()
  {
    beginEditing("a\nb");
    editor.moveCursor(/*line*/0,/*column*/1);
    clearOutput();
    editor.enter(callbacks);
    assert(output()=="lineUnfocused(0)\nlineFocused(1)\n");
    assert(node.lines.size()==3);
  }

  void testEnter2()
  {
    beginEditing("ab");
    editor.moveCursor(/*line*/0,/*column*/1);
    editor.enter();
    assert(node.lines[0].text=="a");
    assert(editor.cursorColumnIndex()==0);
  }

  void testEnter3()
  {
    beginEditing("a=$b=$");
    editor.moveCursor(/*line*/0,/*column*/3);
    editor.enter();
    assert(node.nInputs()==2);
  }
};
}


static void testLeft()
{
  Tester().testLeft1();
  Tester().testLeft2();
}


static void testUp()
{
  Tester().testUp();
}


static void testBackspace()
{
  Tester().testBackspace1();
  Tester().testBackspace2();
  Tester().testBackspace3();
  Tester().testBackspace4();
  Tester().testBackspace5();
}


static void testTextTyped()
{
  Tester().testTextTyped1();
  Tester().testTextTyped2();
}


static void testEnter()
{
  Tester().testEnter1();
  Tester().testEnter2();
  Tester().testEnter3();
}


int main()
{
  Tester().testDownWithNoText();
  Tester().testBeginEditing();
  testLeft();
  Tester().testRight();
  testUp();
  testBackspace();
  Tester().testDown();
  testTextTyped();
  testEnter();
}
