PACKAGES=QtGui QtOpenGL gl glu
CXXFLAGS=-W -Wall -pedantic -std=c++14 -I`pkg-config --cflags $(PACKAGES)` \
 -D_GLIBCXX_DEBUG=1 -MD -MP -g

LDFLAGS=`pkg-config --libs $(PACKAGES)`

all: run_unit_tests main

run_unit_tests: \
  diagrameditor_test.pass \
  nodetexteditor_test.pass \
  linetext_test.pass \
  statementtext_test.pass \
  diagramnode_test.pass \
  diagram_test.pass \
  diagramio_test.pass \
  wrapper_test.pass \
  scene_test.pass \
  world_test.pass

main: main.o diagrameditor.o moc_qtmainwindow.o qtmainwindow.o \
  qtdiagrameditor.o circle.o stringutil.o linetext.o diagramnode.o diagram.o \
  wrapper.o statementtext.o moc_qtcomboboxtreewidgetitemsignalmap.o \
  qtcomboboxtreewidgetitemsignalmap.o qtmenu.o \
  moc_qttreeeditor.o qttreeeditor.o diagramevaluation.o diagramio.o \
  moc_qtdiagrameditor.o qtslot.o moc_qtslot.o defaultdiagrams.o \
  world.o worldwrapper.o charmapperwrapper.o \
  scenewrapper.o charmapper.o qtsceneviewer.o scene.o
	$(CXX) -o $@ $^ $(LDFLAGS) 

moc_%.cpp: %.hpp
	moc-qt4 $^ >$@

%.pass: %
	./$*
	touch $@

diagrameditor_test: diagrameditor_test.o diagrameditor.o stringutil.o \
  linetext.o diagramnode.o diagram.o circle.o statementtext.o \
  diagramevaluation.o
	$(CXX) -o $@ $^ $(LDFLAGS)

nodetexteditor_test: nodetexteditor_test.o linetext.o stringutil.o \
  diagramnode.o statementtext.o
	$(CXX) -o $@ $^ $(LDFLAGS)

linetext_test: linetext_test.o linetext.o stringutil.o
	$(CXX) -o $@ $^ $(LDFLAGS)

statementtext_test: statementtext_test.o statementtext.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramnode_test: diagramnode_test.o diagramnode.o linetext.o stringutil.o \
  statementtext.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagram_test: diagram_test.o diagram.o diagramnode.o linetext.o stringutil.o \
  statementtext.o diagramevaluation.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramio_test: diagramio_test.o diagramio.o diagram.o diagramnode.o \
  linetext.o statementtext.o stringutil.o
	$(CXX) -o $@ $^ $(LDFLAGS)

wrapper_test: wrapper_test.o wrapper.o diagram.o diagramnode.o linetext.o \
  statementtext.o stringutil.o diagramevaluation.o defaultdiagrams.o \
  diagramio.o
	$(CXX) -o $@ $^ $(LDFLAGS)

scene_test: scene_test.o scene.o
	$(CXX) -o $@ $^ $(LDFLAGS)

world_test: world_test.o world.o scene.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.pass moc_*.cpp

-include *.d
