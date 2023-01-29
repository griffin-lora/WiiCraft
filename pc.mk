TARGET := app

SOURCES := $(wildcard src/*.c) $(wildcard src/*.cpp) $(wildcard src/ext/*.c) $(wildcard src/ext/*.cpp) $(wildcard src/game/*.c) $(wildcard src/game/*.cpp) $(wildcard src/gfx/*.c) $(wildcard src/gfx/*.cpp) $(wildcard src/math/*.c) $(wildcard src/math/*.cpp) $(wildcard pc/*.c) $(wildcard pc/ogc/*.c) $(wildcard pc/wiiuse/*.c)
LIBS := -lm
WARNS := -Wall
OBJECTS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
DEPENDS := $(patsubst %.c,%.d,$(patsubst %.cpp,%.d,$(SOURCES)))

override XFLAGS += -O3 -fno-exceptions -I lib -I src -I/opt/devkitpro/libogc/include/ -I/opt/devkitpro/libogc/include/ogc -DPC_PORT -g
CFLAGS = $(XFLAGS) -std=c2x
CXXFLAGS = $(XFLAGS) -std=c++2a

.PHONY: build run clean

build: $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(TARGET).elf $(OBJECTS) $(LIBS)

run: build
	@./$(TARGET).elf

clean:
	$(RM) $(OBJECTS) $(DEPENDS)

-include $(DEPENDS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) $(WARNS) -MMD -MP -c $< -o $@

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) $(WARNS) -MMD -MP -c $< -o $@