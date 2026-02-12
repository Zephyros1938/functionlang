# ==========================================
# Project Settings
# ==========================================
TARGET       := functionlang
SRC_DIR      := src
BUILD_DIR    := build
INCLUDE_DIR  := include

CXX          := g++
CXXFLAGS     := -std=c++23 -Wall -Wextra -Wpedantic -I$(INCLUDE_DIR)

CXXFLAGS     += -g 

SRCS         := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS         := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS         := $(OBJS:.o=.d)

# ==========================================
# Build Rules
# ==========================================

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)
