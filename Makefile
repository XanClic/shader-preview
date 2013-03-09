CXX=g++
CXXFLAGS=$(shell pkg-config --cflags gtkmm-2.4 gtkglext-1.0 pangomm-1.4) -O3 -Wall -Wextra -std=c++11 -DX86_ASSEMBLY -g2
LD=gcc
LDFLAGS=$(shell pkg-config --libs gtkmm-2.4 gtkglext-1.0 pangomm-1.4) -lstdc++ -lm
RM=rm -f

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: all

all: shader-viewer

shader-viewer: $(OBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJS) shader-viewer
