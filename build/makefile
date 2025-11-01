# Compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++26

# libs

LIBS := -lglew32 -lglfw3 -lopengl32 -lgdi32 -luser32 -lpthread  -lmpfr -lgmp -lglew32
LDFLAGS += $(LIBS)

# Build mode: normal, debug, release
MODE ?= normal
RUN ?= false

ifeq ($(MODE),debug)
    CXXFLAGS += -ggdb -O0
    LDFLAGS +=
else ifeq ($(MODE),normal)
    CXXFLAGS += -O1
    LDFLAGS += 
else ifeq ($(MODE),release)
    # Release flags
    CXXFLAGS += -O3 -march=native -funroll-loops -flto \
                -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE -DNDEBUG
    LDFLAGS  += -flto
else
    $(error Unknown build mode '$(MODE)'. Valid options: normal, debug, release)
endif

# --- STATIC BUILD OPTION ---
STATIC ?= false
ifeq ($(STATIC),true)
    # Force static linking of libstdc++, libgcc, and system libs
    LDFLAGS += -static -static-libgcc -static-libstdc++
	CXXFLAGS += -DGLEW_STATIC
endif

# Directories
SRC_DIR := ../src
BUILD_DIR := .
TARGET := XFractal_$(MODE)

INCLUDES += -I../src -I../include -IC:/msys64/ucrt64/include -I C:/msys64/ucrt64/include -I C:/msys64/ucrt64/include/c++/12.2.0 -I C:/msys64/ucrt64/include/c++/12.2.0/x86_64-w64-mingw32 -I C:/msys64/ucrt64/x86_64-w64-mingw32/include

CXXFLAGS += $(INCLUDES)

# Find all source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp) $(wildcard $(SRC_DIR)/*/*/*.cpp)

# Map source files to object files in build/
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Default target
all: $(BUILD_DIR) $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# Compile object files and generate dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPS)

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean
clean:
	rm -rf $(BUILD_DIR)/*.o my_program_*

run: all
	./$(TARGET)

.PHONY: all clean
