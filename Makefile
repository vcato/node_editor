PACKAGES=QtGui QtOpenGL gl glu
CXXFLAGS=-W -Wall -pedantic -std=c++14 -I`pkg-config --cflags $(PACKAGES)` \
 -D_GLIBCXX_DEBUG=1 -MD -MP -g

LDFLAGS=`pkg-config --libs $(PACKAGES)`

all: run_unit_tests main

main: main.o diagrameditor.o
	$(CXX) -o $@ $^ $(LDFLAGS) 

run_unit_tests: \
  diagrameditor_test.pass

diagrameditor_test.pass: diagrameditor_test
	./diagrameditor_test
	touch $@

diagrameditor_test: diagrameditor_test.o diagrameditor.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.pass

-include *.d
