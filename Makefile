.PHONY: all run clean

CC := g++
EXE := execute.exe

BUILD_DIR := ./build
SRC_DIR := src
EXE_DIR := $(BUILD_DIR)/$(EXE)

MKDIR := mkdir -p
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic
# LIBS := no libs so far xd

SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: run

run: $(EXE_DIR)
	./$< $(ARGS)

# COMPILE =====================

$(EXE_DIR): $(OBJS)
	$(CC) $^ -o $@ $(LIBS)

$(BUILD_DIR)/%.o : %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) -c $< -o $@

# =============================
clean:
	rm -r $(BUILD_DIR)
	@echo Ficheiros removidos.