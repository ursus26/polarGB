# Program name
TARGET = polarGB

# C++ compiler
CXX = g++

# C++ compiler flags
CXXFLAGS = -std=c++11 -O2 -Wpedantic -Wall -Wextra -D_FORTIFY_SOURCE=2 -fstack-protector -fPIE

# Linker flags
LDFLAGS = -lboost_program_options -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lpthread

# Project directories
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin
INCLUDE_DIRS = -I./include

# Source modules
MOD_VIDEO = $(SRC_DIR)/video

# Finds the cpp files in the source directory, replaces the source extension with
# the .o extension. Then strip the source directory prefix and add the build
# directory prefix.
_OBJ = $(patsubst $(SRC_DIR)/%, %, $(patsubst %.cpp,%.o,$(wildcard $(SRC_DIR)/*.cpp)))
OBJ = $(patsubst %,$(BUILD_DIR)/%,$(_OBJ))

_OBJ_VIDEO = $(patsubst $(MOD_VIDEO)/%, %, $(patsubst %.cpp,%.o,$(wildcard $(MOD_VIDEO)/*.cpp)))
# OBJ_VIDEO = $(patsubst %,$(BUILD_DIR)/video/%,$(_OBJ_VIDEO)) $(BUILD_DIR)/video/glad.o
OBJ_VIDEO = $(patsubst %,$(BUILD_DIR)/%,$(_OBJ_VIDEO)) $(BUILD_DIR)/glad.o

OBJ := $(OBJ) $(OBJ_VIDEO)

# Standard rule for project compilation. It creates all the directories and
# compiles the entire project.
.PHONY: all
all: directories program

# Rule for creation the directories in the project.
.PHONY: directories
directories:
	@mkdir -p $(BIN_DIR) $(BUILD_DIR) $(BUILD_DIR)/video

# General rule for compiling the entire project.
.PHONY: program
program: $(OBJ) linking


.PHONY: linking
linking:
	@echo "Linking ..."
	@$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/$(TARGET) $(OBJ) $(LDFLAGS)

# Compiles individual cpp files with its header dependencies.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c*
	@echo "Compiling" $< "..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS) $(INCLUDE_DIRS)

# Clean the project by deleting the contentents in the bin and build directory.
.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(BUILD_DIR)/*
	@echo "Project is cleaned up."
