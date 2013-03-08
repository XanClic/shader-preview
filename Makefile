CXX=g++
CXXFLAGS=$(shell pkg-config --cflags gtkglext-1.0) -O3 -Wall -Wextra -std=c++11 -DX86_ASSEMBLY -g2
LD=gcc
LDFLAGS=$(shell pkg-config --libs gtkglext-1.0) -lstdc++ -lm
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
