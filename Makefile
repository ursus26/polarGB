# Program name
TARGET = polarGB

# C++ compiler
CXX = g++

# C++ compiler flags
CXXFLAGS = -g -std=c++11 -Wall -Wextra -pedantic -O2

# Linker flags
LDFLAGS = -g -lboost_program_options

# Project directories
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin

# Finds the cpp files in the source directory, replaces the source extension with
# the .o extension. Then strip the source directory prefix and add the build
# directory prefix.
_OBJ = $(patsubst $(SRC_DIR)/%, %, $(patsubst %.cpp,%.o,$(wildcard $(SRC_DIR)/*.cpp)))
OBJ = $(patsubst %,$(BUILD_DIR)/%,$(_OBJ))

# Standard rule for project compilation. It creates all the directories and
# compiles the entire project.
.PHONY: all
all: directories program

# Rule for creation the directories in the project.
.PHONY: directories
directories:
	@mkdir -p $(BIN_DIR) $(BUILD_DIR)

# General rule for compiling the entire project.
.PHONY: program
program: $(OBJ)
	@echo "Linking ..."
	@$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/$(TARGET) $^ $(LDFLAGS)

# Compiles individual cpp files with its header dependencies.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling " $< "..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

# Clean the project by deleting the contentents in the bin and build directory.
.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(BUILD_DIR)/*
	@echo "Project is cleaned up."
