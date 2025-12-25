# ============================================================================
# Makefile for Go Examples
# ============================================================================
# Build all AI101 Go examples with a single 'make' command.
#
# Usage:
#   make          - Build all examples
#   make clean    - Remove all executables
#   make 01       - Build just example 01
#   make run-01   - Build and run example 01
# ============================================================================

# Go compiler
GO = go

# Directories
BIN_DIR = bin

# All source files (excluding variant files like *_OPENAI.go)
SOURCES = $(wildcard [0-9][0-9]_*.go)
SOURCES := $(filter-out %_ANTHROPIC.go %_OPENAI.go %_DEMETERICS.go %_SAMBA.go,$(SOURCES))

# Generate executable names (strip .go, add bin/ prefix)
EXECUTABLES = $(patsubst %.go,$(BIN_DIR)/%,$(SOURCES))

# Example numbers for convenience targets
EXAMPLES = 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15

# ============================================================================
# Main targets
# ============================================================================

.PHONY: all clean $(EXAMPLES) $(addprefix run-,$(EXAMPLES))

# Default: build all
all: $(BIN_DIR) $(EXECUTABLES)
	@echo ""
	@echo "All examples built successfully!"
	@echo "Run with: ./bin/<example_name>"

# Create bin directory
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Pattern rule: compile any .go file to bin/
$(BIN_DIR)/%: %.go | $(BIN_DIR)
	@echo "Building $@..."
	@$(GO) build -o $@ $<

# Clean up
clean:
	@echo "Cleaning bin/ directory..."
	@rm -rf $(BIN_DIR)
	@echo "Done."

# ============================================================================
# Convenience targets for individual examples
# ============================================================================

# Build specific example by number (e.g., make 01)
01: $(BIN_DIR)/01_basic_chat
02: $(BIN_DIR)/02_system_prompt
03: $(BIN_DIR)/03_prompt_template
04: $(BIN_DIR)/04_vision
05: $(BIN_DIR)/05_safety_check
06: $(BIN_DIR)/06_image_safety_check
07: $(BIN_DIR)/07_prompt_guard
08: $(BIN_DIR)/08_whisper
09: $(BIN_DIR)/09_tavily_search
10: $(BIN_DIR)/10_tavily_extract
11: $(BIN_DIR)/11_tool_use
12: $(BIN_DIR)/12_web_search
13: $(BIN_DIR)/13_code_execution
14: $(BIN_DIR)/14_reasoning
15: $(BIN_DIR)/15_text_to_speech

# Run specific example (e.g., make run-01)
run-01: 01
	@./$(BIN_DIR)/01_basic_chat
run-02: 02
	@./$(BIN_DIR)/02_system_prompt
run-03: 03
	@./$(BIN_DIR)/03_prompt_template
run-04: 04
	@./$(BIN_DIR)/04_vision
run-05: 05
	@./$(BIN_DIR)/05_safety_check
run-06: 06
	@./$(BIN_DIR)/06_image_safety_check
run-07: 07
	@./$(BIN_DIR)/07_prompt_guard
run-08: 08
	@./$(BIN_DIR)/08_whisper
run-09: 09
	@./$(BIN_DIR)/09_tavily_search
run-10: 10
	@./$(BIN_DIR)/10_tavily_extract
run-11: 11
	@./$(BIN_DIR)/11_tool_use
run-12: 12
	@./$(BIN_DIR)/12_web_search
run-13: 13
	@./$(BIN_DIR)/13_code_execution
run-14: 14
	@./$(BIN_DIR)/14_reasoning
run-15: 15
	@./$(BIN_DIR)/15_text_to_speech

# ============================================================================
# Help
# ============================================================================

help:
	@echo "AI101 Go Examples - Makefile"
	@echo ""
	@echo "Usage:"
	@echo "  make          Build all examples"
	@echo "  make clean    Remove all executables"
	@echo "  make 01       Build example 01"
	@echo "  make run-01   Build and run example 01"
	@echo "  make help     Show this help"
	@echo ""
	@echo "Getting Started:"
	@echo "  1. Install Go: https://go.dev/dl/"
	@echo "  2. Run: make"
	@echo "  3. Run: ./bin/01_basic_chat"
	@echo ""
	@echo "Note: Go uses only the standard library - no external dependencies needed!"
