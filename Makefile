PACKAGES=QtGui QtOpenGL gl glu
CXXFLAGS=-W -Wall -pedantic -std=c++14 -I`pkg-config --cflags $(PACKAGES)` \
 -D_GLIBCXX_DEBUG=1 -MD -MP -g

LDFLAGS=`pkg-config --libs $(PACKAGES)`

all: run_unit_tests main

run_unit_tests: \
  diagrameditor_test.pass \
  nodetexteditor_test.pass \
  linetext_test.pass \
  diagramnode_test.pass \
  diagram_test.pass

main: main.o diagrameditor.o moc_qtmainwindow.o qtmainwindow.o \
  qtdiagrameditor.o circle.o stringutil.o linetext.o diagramnode.o diagram.o \
  tree.o
	$(CXX) -o $@ $^ $(LDFLAGS) 

moc_qtmainwindow.cpp: qtmainwindow.hpp
	moc-qt4 $^ >$@

%.pass: %
	./$*
	touch $@

diagrameditor_test: diagrameditor_test.o diagrameditor.o stringutil.o \
  linetext.o diagramnode.o diagram.o
	$(CXX) -o $@ $^ $(LDFLAGS)

nodetexteditor_test: nodetexteditor_test.o linetext.o stringutil.o \
  diagramnode.o
	$(CXX) -o $@ $^ $(LDFLAGS)

linetext_test: linetext_test.o linetext.o stringutil.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramnode_test: diagramnode_test.o diagramnode.o linetext.o stringutil.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagram_test: diagram_test.o diagram.o diagramnode.o linetext.o stringutil.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.pass

-include *.d
