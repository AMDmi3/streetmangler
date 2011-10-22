PYTHON ?= python

LDFLAGS = -L../ $(shell $(PYTHON)-config --libs --ldflags)
CPPFLAGS = -I../lib $(shell $(PYTHON)-config --cflags)

all: _streetmangler.so
clean:
	-rm -f streetmangler.py _streetmangler_wrap.cxx _streetmangler.so

_streetmangler.so: _streetmangler_wrap.o
	$(CC) -o $@ $^ -shared $(LDFLAGS) -lstdc++ -lstreetmangler

.SUFFIXES:
.SUFFIXES: .o .cxx

.cxx.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

%_wrap.cxx: %.i
	swig -python -c++ -Wall $<
