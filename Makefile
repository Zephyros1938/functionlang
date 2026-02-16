# ==========================================
# Project Settings
# ==========================================
TARGET_CLI    := functionlang.out
TARGET_QT     := functionlang-qt.out
SRC_DIR       := src
BUILD_DIR     := build
INCLUDE_DIR   := include

CXX           := g++
CXXFLAGS      := -std=c++23 -Wall -Wextra -Wpedantic -I$(INCLUDE_DIR) -g -fPIC

# QT-Specific Settings
QT_CXXFLAGS   := $(shell pkg-config --cflags Qt6Widgets)
QT_LIBS       := $(shell pkg-config --libs Qt6Widgets)
MOC           := /usr/lib/qt6/moc

MOC_HEADERS   := $(shell grep -l "Q_OBJECT" -r $(SRC_DIR) $(INCLUDE_DIR) 2>/dev/null)
MOC_SRCS      := $(patsubst %, $(BUILD_DIR)/moc_%.cpp, $(notdir $(MOC_HEADERS:.hpp=)))
MOC_OBJS      := $(MOC_SRCS:.cpp=.o)

ALL_SRCS      := $(shell find $(SRC_DIR) -name '*.cpp')
QT_SRCS       := $(shell find $(SRC_DIR)/qt -name '*.cpp')
TERM_SRCS     := $(shell find $(SRC_DIR)/term -name '*.cpp')
# Common sources are anything NOT in qt/ or term/
COMMON_SRCS   := $(filter-out $(QT_SRCS) $(TERM_SRCS), $(ALL_SRCS))

# Convert to objects
COMMON_OBJS   := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(COMMON_SRCS))
QT_OBJS       := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(QT_SRCS))
TERM_OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TERM_SRCS))

# ==========================================
# Build Rules
# ==========================================

.PHONY: all qt clean run

# Default target only builds CLI
all: $(TARGET_CLI)

qt: $(TARGET_QT)

# Terminal App: Only uses Common + Term objects
$(TARGET_CLI): $(COMMON_OBJS) $(TERM_OBJS)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built Terminal version: $@"

# QT App: Uses Common + Qt objects + MOC objects
$(TARGET_QT): $(COMMON_OBJS) $(QT_OBJS) $(MOC_OBJS)
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(QT_LIBS)
	@echo "Built QT version: $@"

# Rule to run MOC
# This simplified rule handles headers found anywhere in the project
$(BUILD_DIR)/moc_%.cpp:
	@mkdir -p $(dir $@)
	@$(eval HEADER := $(shell find $(SRC_DIR) $(INCLUDE_DIR) -name "$*.hpp"))
	@echo "MOC-ing $(HEADER)..."
	@$(MOC) $(HEADER) -o $@

$(BUILD_DIR)/moc_%.o: $(BUILD_DIR)/moc_%.cpp
	@echo "Compiling generated MOC file $<..."
	@$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

# Generic Compile Rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(if $(findstring /qt/,$<),$(QT_CXXFLAGS)) -MMD -MP -c $< -o $@

-include $(shell find $(BUILD_DIR) -name '*.d' 2>/dev/null)

clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR) $(TARGET_CLI) $(TARGET_QT)

run: $(TARGET_CLI)
	./$(TARGET_CLI)
