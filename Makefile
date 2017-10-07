# Clean engine Makefile
#

default: all

# Pick one of:
#   linux
#   macos
#   windows

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
  OS=macos
else ifeq ($(UNAME),Linux)
  OS=linux
else ifeq ($(UNAME),FreeBSD)
  OS=bsd
else ifeq ($(UNAME),OpenBSD)
  OS=bsd
else ifneq (,$(findstring MINGW32_NT,$(UNAME)))
  OS=win
else
  $(error unknown os $(UNAME))
endif

# Build directory
#

BUILD=build
INCLUDE=src

RM=rm
MAKE=make
ifneq ($(UNAME),FreeBSD)
  CXX=g++
else
  CXX=c++
endif

CXXFLAGS=-std=c++14
CXXWFLAGS=-Wall -Wextra

ifeq ($(DEBUG),1)
  CXXFLAGS+=--debug
endif

# Files to build
#

PROGRAM_NAME=circular_buffer
PROGRAM_OFILES=src/circular_buffer.o

# Libraries to link
#

SDL2_VER=SDL2-2.0.6

ifeq ($(OS),$(filter $(OS),linux macos))
  CXXFLAGS+=-Ioutside/$(SDL2_VER)/include # -Loutside/$(SDL2_VER)/build
  SDL2_LIB=outside/$(SDL2_VER)/build/libSDL2.la
  PROGRAM_OUTSIDE+=$(SDL2_LIB)
  PROGRAM_LIBS=-lSDL2 -lSDL2main
endif

ifeq ($(OS),linux)
  PROGRAM_LIBS+=-lX11
endif

# Make targets
#

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): clean $(BUILD)/$(PROGRAM_NAME)

$(BUILD)/$(PROGRAM_NAME): $(PROGRAM_OUTSIDE) $(PROGRAM_OFILES)
	@echo "    BUILD  $(BUILD)/$(PROGRAM_NAME)"
	@mkdir -p $(BUILD)
	@$(CXX) $(CXXFLAGS) -o $(BUILD)/$(PROGRAM_NAME) $(PROGRAM_OFILES) $(PROGRAM_LIBS)

%.o: %.cpp
	@echo "    CXX    $@"
	@$(CXX) -c $(CXXWFLAGS) $(CXXFLAGS) -o $@ $<

$(SDL2_LIB):
	@echo "    CXX    $(SDL2_VER)"
	cd outside && tar -xzf $(SDL2_VER).tar.gz
	cd outside/$(SDL2_VER) && ./configure
	$(MAKE) -C outside/$(SDL2_VER)

clean:
	$(RM) -f src/*.o build/*
	find . -name '*~' -delete
# Misc
#

TAGS=.tags \
     .etags

tags: etags

etags:
	etags -f .etags $$(find ./src -name '*.cpp' -or -name '*.hpp') || true

.PHONY: clean full-clean etags tags
