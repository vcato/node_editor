PACKAGES=QtGui QtOpenGL gl glu
CXXFLAGS=-W -Wall -pedantic -std=c++14 -I`pkg-config --cflags $(PACKAGES)` \
 -D_GLIBCXX_DEBUG=1 -MD -MP -ggdb3

LDFLAGS=`pkg-config --libs $(PACKAGES)`

all: run_unit_tests build_manual_tests main

run_unit_tests: \
  optional_test.pass \
  any_test.pass \
  charmapperobjects_test.pass \
  evaluateexpression_test.pass \
  diagramevaluation_test.pass \
  diagrameditor_test.pass \
  nodetexteditor_test.pass \
  linetext_test.pass \
  statementtext_test.pass \
  diagramnode_test.pass \
  diagram_test.pass \
  diagramio_test.pass \
  wrapper_test.pass \
  scene_test.pass \
  world_test.pass \
  wrapperstate_test.pass \
  scenewrapper_test.pass \
  worldwrapper_test.pass \
  charmapper_test.pass \
  treeeditor_test.pass \
  mainwindow_test.pass

build_manual_tests: \
  qtscenewindow_manualtest \
  qtdiagrameditorwindow_manualtest

main: main.o diagrameditor.o \
  qtmainwindow.o          moc_qtmainwindow.o \
  qtspinbox.o             moc_qtspinbox.o \
  qttreeeditor.o          moc_qttreeeditor.o \
  qtdiagrameditor.o       moc_qtdiagrameditor.o \
  qtslot.o                moc_qtslot.o \
  qtcombobox.o            moc_qtcombobox.o \
  qtlineedit.o            moc_qtlineedit.o \
  qtdiagrameditorwindow.o \
  mainwindow.o \
  circle.o stringutil.o linetext.o diagramnode.o diagram.o \
  wrapper.o statementtext.o generatename.o \
  qtmenu.o \
  treeeditor.o \
  diagramevaluation.o diagramio.o \
  defaultdiagrams.o \
  world.o worldwrapper.o charmapperwrapper.o \
  scenewrapper.o charmapper.o qtsceneviewer.o scene.o draw.o qtworld.o \
  qtscenewindow.o qttreewidgetitem.o scenewindow.o \
  qtscenetree.o scenetree.o sceneviewer.o wrapperutil.o evaluateexpression.o \
  wrapperstate.o makediagram.o point2d.o vector2d.o diagrameditorwindow.o \
  maybepoint2d.o charmapperobjects.o sceneobjects.o point2dobject.o \
  globalvec.o printindent.o
	$(CXX) -o $@ $^ $(LDFLAGS) 

moc_%.cpp: %.hpp
	moc-qt4 $^ >$@

%.pass: %
	./$*
	touch $@

optional_test: optional_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

any_test: any_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

charmapperobjects_test: charmapperobjects_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

evaluateexpression_test: evaluateexpression_test.o evaluateexpression.o \
  maybepoint2d.o charmapperobjects.o scene.o generatename.o sceneobjects.o \
  point2dobject.o globalvec.o point2d.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramevaluation_test: diagramevaluation_test.o diagram.o diagramnode.o \
  linetext.o statementtext.o stringutil.o evaluateexpression.o \
  diagramevaluation.o defaultdiagrams.o diagramio.o makediagram.o \
  maybepoint2d.o point2dobject.o point2d.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagrameditor_test: diagrameditor_test.o diagrameditor.o stringutil.o \
  linetext.o diagramnode.o diagram.o circle.o statementtext.o \
  diagramevaluation.o evaluateexpression.o diagramio.o point2d.o \
  vector2d.o
	$(CXX) -o $@ $^ $(LDFLAGS)

nodetexteditor_test: nodetexteditor_test.o linetext.o stringutil.o \
  diagramnode.o statementtext.o evaluateexpression.o
	$(CXX) -o $@ $^ $(LDFLAGS)

linetext_test: linetext_test.o linetext.o stringutil.o evaluateexpression.o
	$(CXX) -o $@ $^ $(LDFLAGS)

statementtext_test: statementtext_test.o statementtext.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramnode_test: diagramnode_test.o diagramnode.o linetext.o stringutil.o \
  statementtext.o evaluateexpression.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagram_test: diagram_test.o diagram.o diagramnode.o linetext.o stringutil.o \
  statementtext.o diagramevaluation.o evaluateexpression.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramio_test: diagramio_test.o diagramio.o diagram.o diagramnode.o \
  linetext.o statementtext.o stringutil.o evaluateexpression.o makediagram.o
	$(CXX) -o $@ $^ $(LDFLAGS)

wrapper_test: wrapper_test.o wrapper.o diagram.o diagramnode.o linetext.o \
  statementtext.o stringutil.o diagramevaluation.o defaultdiagrams.o \
  diagramio.o wrapperutil.o evaluateexpression.o makediagram.o
	$(CXX) -o $@ $^ $(LDFLAGS)

scene_test: scene_test.o scene.o generatename.o
	$(CXX) -o $@ $^ $(LDFLAGS)

world_test: world_test.o world.o scene.o scenewindow.o scenetree.o \
  generatename.o charmapper.o defaultdiagrams.o diagram.o diagramio.o \
  diagramnode.o linetext.o statementtext.o stringutil.o sceneviewer.o \
  diagramevaluation.o evaluateexpression.o makediagram.o maybepoint2d.o \
  sceneobjects.o charmapperobjects.o point2dobject.o globalvec.o \
  point2d.o
	$(CXX) -o $@ $^ $(LDFLAGS)

wrapperstate_test: wrapperstate_test.o wrapperstate.o stringutil.o wrapper.o \
  printindent.o
	$(CXX) -o $@ $^ $(LDFLAGS)

scenewrapper_test: scenewrapper_test.o scenewrapper.o scene.o wrapperutil.o \
  generatename.o wrapperstate.o stringutil.o wrapper.o printindent.o
	$(CXX) -o $@ $^ $(LDFLAGS)

worldwrapper_test: worldwrapper_test.o world.o scene.o worldwrapper.o \
  scenewrapper.o charmapperwrapper.o charmapper.o diagram.o defaultdiagrams.o \
  diagramnode.o diagramio.o linetext.o statementtext.o stringutil.o \
  wrapperutil.o wrapper.o scenewindow.o scenetree.o generatename.o \
  sceneviewer.o diagramevaluation.o evaluateexpression.o wrapperstate.o \
  makediagram.o maybepoint2d.o charmapperobjects.o sceneobjects.o \
  point2dobject.o globalvec.o point2d.o printindent.o
	$(CXX) -o $@ $^ $(LDFLAGS)

charmapper_test: charmapper_test.o scene.o charmapper.o defaultdiagrams.o \
  diagram.o diagramio.o diagramnode.o linetext.o statementtext.o \
  stringutil.o generatename.o diagramevaluation.o evaluateexpression.o \
  makediagram.o maybepoint2d.o point2dobject.o charmapperobjects.o \
  sceneobjects.o globalvec.o point2d.o
	$(CXX) -o $@ $^ $(LDFLAGS)

treeeditor_test: treeeditor_test.o treeeditor.o wrapper.o wrapperutil.o \
  diagrameditor.o diagram.o diagramnode.o circle.o linetext.o \
  statementtext.o stringutil.o fakediagrameditorwindows.o \
  evaluateexpression.o wrapperstate.o diagramio.o point2d.o vector2d.o \
  diagrameditorwindow.o printindent.o
	$(CXX) -o $@ $^ $(LDFLAGS)

mainwindow_test: mainwindow_test.o mainwindow.o worldwrapper.o \
  charmapperwrapper.o charmapper.o \
  scenewrapper.o scene.o scenewindow.o \
  defaultdiagrams.o diagram.o diagramio.o diagramnode.o linetext.o \
  statementtext.o stringutil.o \
  treeeditor.o wrapper.o world.o scenetree.o generatename.o sceneviewer.o \
  wrapperutil.o diagrameditor.o circle.o fakediagrameditorwindows.o \
  diagramevaluation.o evaluateexpression.o wrapperstate.o makediagram.o \
  point2d.o vector2d.o diagrameditorwindow.o maybepoint2d.o \
  charmapperobjects.o sceneobjects.o point2dobject.o globalvec.o \
  printindent.o
	$(CXX) -o $@ $^ $(LDFLAGS)

qtscenewindow_manualtest: qtscenewindow_manualtest.o qtscenewindow.o \
  qtsceneviewer.o draw.o scene.o qttreewidgetitem.o qtscenetree.o \
  scenetree.o scenewindow.o generatename.o sceneviewer.o
	$(CXX) -o $@ $^ $(LDFLAGS)

qtdiagrameditorwindow_manualtest: qtdiagrameditorwindow_manualtest.o \
  qtdiagrameditor.o moc_qtdiagrameditor.o diagrameditor.o diagramio.o \
  qtmenu.o draw.o diagram.o diagramevaluation.o diagramnode.o circle.o \
  qtslot.o moc_qtslot.o linetext.o statementtext.o stringutil.o \
  qtdiagrameditorwindow.o evaluateexpression.o point2d.o vector2d.o \
  diagrameditorwindow.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.pass moc_*.cpp

-include *.d
