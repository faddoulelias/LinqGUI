.PHONY = all clean run

# Compiler
CXX := g++
CFLAGS := -Wall -g -std=c++17
SDLFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf

# Directories
BUILD  := ./build
OBJECT := ./build/obj
SOURCE := ./src
HEADER := ./src/headers

# Change here ----------------------------------
DEPS := ../Client ../Server
# ----------------------------------------------

# Files
SRCS := $(wildcard $(SOURCE)/*.cpp)
OBJS := $(patsubst $(SOURCE)/%.cpp, $(OBJECT)/%.o, $(SRCS))
EXEC := $(BUILD)/Flow

DEPS_OBJS := $(foreach dep, $(DEPS), $(wildcard $(dep)/build/obj/*.o)) 
DEPS_OBJS := $(filter-out %Main.o, $(DEPS_OBJS))

# Makefile commands
all : $(EXEC)

run:$(EXEC)
	$(EXEC) $(ARGS)

$(EXEC) : $(OBJS)
	$(foreach dep, $(DEPS), $(MAKE) -C $(dep) all;) 
	$(CXX) $(CFLAGS) -o $(EXEC) $(OBJS) $(DEPS_OBJS) $(SDLFLAGS)

$(OBJECT)/%.o : $(SOURCE)/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^ $(SDLFLAGS) -I$(HEADER) $(foreach dep, $(DEPS), -I$(dep)/src/headers)

clean:
	rm -f $(EXEC) $(OBJS)
