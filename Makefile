.PHONY: all run clean

CC := g++
EXE := execute.exe
BUILD_DIR := ./build
SRC_DIR := src
EXE_DIR := $(BUILD_DIR)/$(EXE)
MKDIR := mkdir -p

# Caminhos para o FTXUI
FTXUI_DIR := ./lib/FTXUI
FTXUI_INCLUDE := $(FTXUI_DIR)/include
FTXUI_BUILD := $(FTXUI_DIR)/build

# Flags de compilação
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -I$(FTXUI_INCLUDE)

# Bibliotecas FTXUI - A ORDEM É IMPORTANTE!
LIBS := -L$(FTXUI_BUILD) -lftxui-component -lftxui-dom -lftxui-screen

SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: comp
run: $(EXE_DIR)
	./$< $(ARGS)

# COMPILE =====================
comp: $(EXE_DIR)

$(EXE_DIR): $(OBJS)
	$(CC) $^ -o $@ $(LIBS)

$(BUILD_DIR)/%.o : %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) -c $< -o $@

# =============================
clean:
	rm -rf $(BUILD_DIR)
	@echo "Ficheiros removidos."
