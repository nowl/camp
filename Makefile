CXX = g++
CXXFLAGS = -Wall -g -O2 -fPIC -std=c++0x -DNOCLEANUP
INCLUDES = $(shell sdl-config --cflags) $(shell lua-config --include) -I../lapis
LDFLAGS = -shared

LIBS = $(shell sdl-config --libs) $(shell lua-config --libs) -lSDL_ttf -lSDL_image -lSDL_mixer -lGL -lGLU

EXECSRCS = \
	cp437.cpp \
	test.cpp

EXECOBJS = $(EXECSRCS:.cpp=.o)

MAINEXEC = main

all: $(MAINLIB) $(MAINEXEC)

.SUFFIXES: .cpp .o .i

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: depend clean all

$(MAINEXEC): $(EXECOBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAINEXEC) $(EXECOBJS) -L. -llapis -Wl,-rpath,'$$ORIGIN'

#lapis_wrap.c: lapis.i
#	swig -lua lapis.i

clean:
	rm -f *.o *~ $(MAINEXEC)
 #lapis_wrap.c

depend: $(EXECSRCS)
	$(CXX) -M $(CXXFLAGS) $(INCLUDES) $^ > $@

include depend
