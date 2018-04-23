#include "fakediagrameditor.hpp"
#include "diagrameditorwindow.hpp"


struct FakeDiagramEditorWindow;


struct FakeDiagramEditorWindows :
  std::vector<std::unique_ptr<FakeDiagramEditorWindow>>
{
  FakeDiagramEditorWindow &create()
  {
    push_back(std::make_unique<FakeDiagramEditorWindow>(*this));
    return *back();
  }
};


struct FakeDiagramEditorWindow : DiagramEditorWindow {
  FakeDiagramEditorWindows &all_windows;

  FakeDiagramEditorWindow(FakeDiagramEditorWindows &all_windows_arg)
  : all_windows(all_windows_arg)
  {
  }

  virtual DiagramEditor &diagramEditor()
  {
    return diagram_editor;
  }

  void userClosesWindow();
  void forceClose();

  FakeDiagramEditor diagram_editor;
};
