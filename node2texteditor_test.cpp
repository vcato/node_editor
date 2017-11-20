#include "node2texteditor.hpp"

#include <cassert>
#include <sstream>


using std::vector;
using std::string;
using std::istringstream;


static vector<string> split(const string &text)
{
  istringstream stream(text);
  vector<string> result;
  string line;
  while (getline(stream,line)) {
    result.push_back(line);
  }
  return result;
}


static void setText(Node2 &node,const std::string &text)
{
  if (text=="") {
    node.lines.resize(1,Node2::Line(""));
    node.updateInputsAndOutputs();
    return;
  }

  vector<string> lines = split(text);

  for (const auto &line : lines) {
    node.lines.push_back(Node2::Line(line));
  }

  if (endsWith(text,"\n")) {
    node.lines.push_back(Node2::Line(""));
  }

  node.updateInputsAndOutputs();
}


namespace {
struct Tester {
  Node2TextEditor editor;
  Node2 node;

  void testDownWithNoText()
  {
    editor.beginEditing(node);
    editor.down();
  }

  void beginEditing(const char *text)
  {
    setText(node,text);
    editor.beginEditing(node);
  }

  void testBeginEditing()
  {
    beginEditing("a");
    assert(editor.cursor_line_index==0);
    assert(editor.cursor_column_index==1);
  }

  void testLeft1()
  {
    beginEditing("aa");
    editor.cursor_line_index = 0;
    editor.cursor_column_index = 2;
    editor.left();
    assert(editor.cursor_column_index==1);
    editor.left();
    assert(editor.cursor_column_index==0);
    editor.left();
    assert(editor.cursor_column_index==0);
  }

  void testLeft2()
  {
    beginEditing("a\naa");
    editor.cursor_line_index = 1;
    editor.cursor_column_index = 2;
    editor.up();
    editor.left();
    assert(editor.cursor_column_index==0);
  }

  void testRight()
  {
    beginEditing("aa");
    editor.cursor_line_index = 0;
    editor.cursor_column_index = 0;
    editor.right();
    assert(editor.cursor_column_index==1);
    editor.right();
    assert(editor.cursor_column_index==2);
    editor.right();
    assert(editor.cursor_column_index==2);
  }

  void testUp()
  {
    beginEditing("a\nab");
    editor.cursor_line_index = 1;
    editor.cursor_column_index = 2;
    editor.up();
    editor.textTyped("c");
  }

  void testBackspace1()
  {
    beginEditing("aa");
    editor.cursor_line_index = 0;
    editor.cursor_column_index = 2;
    editor.backspace();
    assert(node.lines[0].text=="a");
    assert(editor.cursor_column_index==1);
    editor.backspace();
    assert(node.lines[0].text=="");
    assert(editor.cursor_column_index==0);
    editor.backspace();
  }

  void testBackspace2()
  {
    beginEditing("ab");
    editor.cursor_line_index = 0;
    editor.cursor_column_index = 1;
    editor.backspace();
    assert(node.lines[0].text=="b");
    editor.backspace();
  }

  void testBackspace3()
  {
    beginEditing("a\n");
    assert(node.lines.size()==2);
    editor.cursor_line_index = 1;
    editor.cursor_column_index = 0;
    editor.backspace();
    assert(editor.cursor_column_index==1);
    assert(editor.cursor_line_index==0);
  }

  void testBackspace4()
  {
    beginEditing("a = $\n$ = a");
    assert(node.nOutputs()==1);
    editor.cursor_line_index = 1;
    editor.cursor_column_index = 0;
    editor.backspace();
    assert(node.lines.size()==1);
    assert(node.nOutputs()==0);
  }

  void testBackspace5()
  {
    beginEditing("a\naa");
    editor.cursor_line_index = 1;
    editor.cursor_column_index = 2;
    editor.up();
    editor.backspace();
  }

  void testDown()
  {
    beginEditing("a\nb");
    assert(node.lines.size()==2);
    editor.cursor_line_index = 0;
    editor.down();
    assert(editor.cursor_line_index==1);
    editor.down();
    assert(editor.cursor_line_index==1);
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
    editor.cursor_column_index = 1;
    editor.textTyped("=");
    assert(node.lines[0].text=="a=b");
    assert(editor.cursor_column_index==2);
  }

  void testEnter1()
  {
    beginEditing("a\nb");
    editor.cursor_line_index = 0;
    editor.cursor_column_index = 1;
    editor.enter();
    assert(node.lines.size()==3);
  }

  void testEnter2()
  {
    beginEditing("ab");
    editor.cursor_line_index = 0;
    editor.cursor_column_index = 1;
    editor.enter();
    assert(node.lines[0].text=="a");
    assert(editor.cursor_column_index==0);
  }

  void testEnter3()
  {
    beginEditing("a=$b=$");
    editor.cursor_line_index = 0;
    editor.cursor_column_index = 3;
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
