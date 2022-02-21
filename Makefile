##
# zosheet desktop application
#
# @file
# @version 0.1

SRC_PATH := ./src
INCLUDE_PATH := ./include
BIN_PATH := ./bin
BUILD_PATH := ./build
TESTS_PATH := ./tests

CXX := g++
CXX_FLAGS := -Wall -g -I$(INCLUDE_PATH) `pkg-config gtkmm-4.0 --cflags --libs` -std=c++17

PROJECT_NAME := zosheet

OBJ_MODULES := $(addsuffix .o, main window widgets objects parser)

makedirs:
	@if [ ! -d $(BUILD_PATH) ] || [ ! -d $(BIN_PATH) ]; then \
		echo "[INFO]: creating 'build' and 'bin' directories"; \
		mkdir -p $(BUILD_PATH) $(BIN_PATH); \
	fi


$(BUILD_PATH)/%.o: $(SRC_PATH)/%.cpp $(INCLUDE_PATH)/%.h
	@echo "[INFO]: Building '$<' module"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@


$(BUILD_PATH)/main.o: $(SRC_PATH)/main.cpp
	@echo "[INFO]: Building '$<' module"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

build: makedirs $(addprefix $(BUILD_PATH)/, $(OBJ_MODULES))
	@echo "[INFO]: Linking "$(PROJECT_NAME)" application"
	@$(CXX) $(CXX_FLAGS) $(filter %.o, $^) -o $(BIN_PATH)/$(PROJECT_NAME)


TESTS_OBJ := $(addsuffix .o, $(basename $(notdir $(wildcard $(TESTS_PATH)/*.cpp))))

$(BUILD_PATH)/%.o: $(TESTS_PATH)/%.cpp
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

build_tests: $(addprefix $(BUILD_PATH)/, $(TESTS_OBJ))
	@echo "[INFO]: Building tests..."
	@$(CXX) $(CXX_FLAGS) $^ -o $(BIN_PATH)/tests

tests: makedirs build_tests
	@echo "[INFO]: Running tests"
	@./$(BIN_PATH)/tests

# end
