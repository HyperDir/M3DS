.SUFFIXES:

LIB_NAME	:= m3ds

C_VERSION 	:= 23
CXX_VERSION := 26

#---------------------------------------------------------------------------------
# Environment Setup
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPRO)),)
	$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif

ifeq ($(strip $(DEVKITARM)),)
	$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

BUILD_DIR 		:= build
SOURCES_DIR		:= source
INCLUDE_DIR 	:= include
EMBED_DIR		:= $(BUILD_DIR)
GFX_DIR			:= gfx
ifndef (OUTPUT_DIR)
	OUTPUT_DIR 	:= lib
endif

#---------------------------------------------------------------------------------
# Flags
#---------------------------------------------------------------------------------
ARCH_FLAGS 		:= -march=armv6k -mtune=mpcore -mfloat-abi=hard -mword-relocations

WARNINGS		:= -Wall -Werror -Wextra -Wconversion -Wpedantic -Wno-psabi

MAKEFLAGS 		:= -j14 --silent
COMMON_FLAGS 	:= $(WARNINGS) -ffast-math $(ARCH_FLAGS) -D__3DS__
C_FLAGS			:= $(COMMON_FLAGS) -std=c$(C_VERSION)
CXX_FLAGS		:= $(COMMON_FLAGS) -std=c++$(CXX_VERSION) -fno-rtti -fno-exceptions

DEBUG_FLAGS		:= -O0 -g -DDEBUG
RELEASE_FLAGS	:= -O2 -DNDEBUG -s -fomit-frame-pointer

ASM_FLAGS 		:= $(ARCH_FLAGS)
LD_FLAGS		:= -specs=3dsx.specs $(ARCH_FLAGS) -z noexecstack

ifeq ($(OS),Windows_NT)
	DEVKITPRO := $(shell echo "$(DEVKITPRO)" | sed -e 's|^/opt|C:|')
	DEVKITARM := $(shell echo "$(DEVKITARM)" | sed -e 's|^/opt|C:|')
endif

PORTLIBS		:= $(DEVKITPRO)/portlibs/3ds
LIBCTRU			:= $(DEVKITPRO)/libctru

LIB_DIRS		:= $(PORTLIBS) $(LIBCTRU)

#---------------------------------------------------------------------------------
# Build Variable Setup
#---------------------------------------------------------------------------------
recurse = $(shell find $2 -type $1 -name '$3' 2> /dev/null)

C_FILES			:= $(foreach dir,$(SOURCES_DIR),$(call recurse,f,$(dir),*.c))
CXX_FILES		:= $(foreach dir,$(SOURCES_DIR),$(call recurse,f,$(dir),*.cpp))
PICA_FILES		:= $(foreach dir,$(SOURCES_DIR),$(call recurse,f,$(dir),*.pica))

GFX_FILES		:= $(foreach dir,$(GFX_DIR),$(call recurse,f,$(dir),*.*))

GFX_EXTENSIONS	:= $(addprefix %.,PNG png JPG jpg JPEG jpeg BMP bmp)
T3X_FILES 		:= $(addsuffix .t3x, $(addprefix $(BUILD_DIR)/, $(filter $(GFX_EXTENSIONS),$(GFX_FILES))))

O_FILES 		:= $(patsubst $(SOURCES_DIR)/%,$(BUILD_DIR)/%,$(addsuffix .o, $(basename $(C_FILES) $(CXX_FILES))))
OD_FILES 		:= $(patsubst $(SOURCES_DIR)/%,$(BUILD_DIR)/%,$(addsuffix _DEBUG.o, $(basename $(C_FILES) $(CXX_FILES))))
BIN_FILES		:= $(patsubst $(SOURCES_DIR)/%,$(BUILD_DIR)/%,$(addsuffix .bin, $(basename $(basename $(PICA_FILES)))))

INCLUDE			:= $(foreach dir,$(INCLUDE_DIR),-I./$(dir)) $(foreach dir,$(LIB_DIRS),-isystem $(dir)/include)
EMBED			:= $(foreach dir,$(EMBED_DIR),--embed-dir=./$(dir))

CC		:= $(DEVKITARM)/bin/arm-none-eabi-gcc
CXX		:= $(DEVKITARM)/bin/arm-none-eabi-g++
AR		:= $(DEVKITARM)/bin/arm-none-eabi-gcc-ar
LD		:= $(CXX)

# msys2 make and mingw32-make don't agree on PATH format on Windows
PATH := $(DEVKITPRO)/tools/bin/:$(DEVKITARM)/bin/:$(PATH)
PATH := $(subst C:/,/c/,$(DEVKITPRO)/tools/bin/:$(DEVKITARM)/bin/):$(PATH)

.PHONY: lib clean all debug release

all: debug release

# Shaders
$(BUILD_DIR)/%.bin: $(SOURCES_DIR)/%.v.pica
	$(info Compiling $^)
	@mkdir -p $(dir $@)
	@picasso -o $@ $^

$(BUILD_DIR)/%.bin: $(SOURCES_DIR)/%.v.pica $(SOURCES_DIR)/%.g.pica
	$(info Compiling $^)
	@mkdir -p $(dir $@)
	@picasso -o $@ $^

# Textures
$(BUILD_DIR)/%.t3x: %
	$(info Processing texture $<)
	@mkdir -p $(dir $@)
	@tex3ds --atlas -f rgba8888 -z -i $< -o $@ > /dev/null 2>&1



# Release
$(OUTPUT_DIR)/%.a: $(O_FILES)
	$(info Archiving $(notdir $@))
	@mkdir -p $(dir $@)
	@$(AR) rcs $@ $(O_FILES)
	$(info Compiled $(LIB_NAME) in release mode!)

$(BUILD_DIR)/%.o: $(SOURCES_DIR)/%.c $(BIN_FILES) $(T3X_FILES)
	$(info Compiling $< for release)
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP -MF $(BUILD_DIR)/$*.d $(C_FLAGS) $(RELEASE_FLAGS) $(INCLUDE) $(EMBED) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCES_DIR)/%.cpp $(BIN_FILES) $(T3X_FILES)
	$(info Compiling $< for release)
	@mkdir -p $(dir $@)
	@$(CXX) -MMD -MP -MF $(BUILD_DIR)/$*.d $(CXX_FLAGS) $(RELEASE_FLAGS) $(INCLUDE) $(EMBED) -c $< -o $@



# Debug
$(OUTPUT_DIR)/%d.a: $(OD_FILES)
	$(info Archiving $(notdir $@))
	@mkdir -p $(dir $@)
	@$(AR) rcs $@ $(OD_FILES)
	$(info Compiled $(LIB_NAME) in debug mode!)

$(BUILD_DIR)/%_DEBUG.o: $(SOURCES_DIR)/%.c $(BIN_FILES) $(T3X_FILES)
	$(info Compiling $< for debug)
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP -MF $(BUILD_DIR)/$*_DEBUG.d $(C_FLAGS) $(DEBUG_FLAGS) $(INCLUDE) $(EMBED) -c $< -o $@

$(BUILD_DIR)/%_DEBUG.o: $(SOURCES_DIR)/%.cpp $(BIN_FILES) $(T3X_FILES)
	$(info Compiling $< for debug)
	@mkdir -p $(dir $@)
	@$(CXX) -MMD -MP -MF $(BUILD_DIR)/$*_DEBUG.d $(CXX_FLAGS) $(DEBUG_FLAGS) $(INCLUDE) $(EMBED) -c $< -o $@



debug: $(OUTPUT_DIR)/lib$(LIB_NAME)d.a

release: $(OUTPUT_DIR)/lib$(LIB_NAME).a

lib: release

install: debug release
	$(info Installing $(LIB_NAME) to $(DEVKITPRO)/portlibs/3ds...)
	@cp -rv include lib $(DEVKITPRO)/portlibs/3ds > /dev/null
	$(info Installed $(LIB_NAME)!)

clean:
	@echo Cleaning $(LIB_NAME)...
	@rm -rf $(BUILD_DIR) $(OUTPUT_DIR)

-include $(O_FILES:.o=.d) $(OD_FILES:.o=.d)
