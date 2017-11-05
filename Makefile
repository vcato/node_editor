PACKAGES=QtGui QtOpenGL gl glu
CXXFLAGS=-W -Wall -pedantic -std=c++14 -I`pkg-config --cflags $(PACKAGES)` \
 -D_GLIBCXX_DEBUG=1

LDFLAGS=`pkg-config --libs $(PACKAGES)`

main: main.o
	$(CXX) -o $@ $^ $(LDFLAGS) 

clean:
	rm -f *.o
