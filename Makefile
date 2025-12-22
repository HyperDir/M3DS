.SUFFIXES:

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

TARGET			:= $(shell basename $(CURDIR))
BUILD_DIR 		:= build
SOURCES_DIR		:= source
INCLUDE_DIR 	:= include
EMBED_DIR		:= $(BUILD_DIR)
ifndef (OUTPUT_DIR)
	OUTPUT_DIR 	:= lib
endif

#---------------------------------------------------------------------------------
# Flags
#---------------------------------------------------------------------------------
ARCH_FLAGS 		:= -march=armv6k -mtune=mpcore -mfloat-abi=hard

WARNINGS		:= -Wall -Werror -Wextra -Wconversion -Wpedantic -Wno-psabi

MAKEFLAGS 		:= -j14 --silent
COMMON_FLAGS 	:= $(WARNINGS) -O2 -mword-relocations -fomit-frame-pointer -ffast-math $(ARCH_FLAGS) -D__3DS__
C_FLAGS			:= $(COMMON_FLAGS) -std=c$(C_VERSION)
CXX_FLAGS		:= $(COMMON_FLAGS) -std=c++$(CXX_VERSION) -fno-rtti -fno-exceptions

ASM_FLAGS 		:= $(ARCH_FLAGS)
LD_FLAGS		:= -specs=3dsx.specs $(ARCH_FLAGS) -z noexecstack

ifeq ($(OS),Windows_NT)
	DEVKITPRO := $(shell echo "$(DEVKITPRO)" | sed -e 's|^/opt|C:|')
	DEVKITARM := $(shell echo "$(DEVKITARM)" | sed -e 's|^/opt|C:|')
endif

PORTLIBS		:= $(DEVKITPRO)/portlibs/3ds
LIBCTRU			:= $(DEVKITPRO)/libctru

LIBS			:= -lcitro2d -lcitro3d -lctru -lm
LIB_DIRS		:= $(PORTLIBS) $(LIBCTRU) ./lib

#---------------------------------------------------------------------------------
# Build Variable Setup
#---------------------------------------------------------------------------------
recurse = $(shell find $2 -type $1 -name '$3' 2> /dev/null)

C_FILES			:= $(foreach dir,$(SOURCES_DIR),$(call recurse,f,$(dir),*.c))
CXX_FILES		:= $(foreach dir,$(SOURCES_DIR),$(call recurse,f,$(dir),*.cpp))
S_FILES			:= $(foreach dir,$(SOURCES_DIR),$(call recurse,f,$(dir),*.s))
PICA_FILES		:= $(foreach dir,$(SOURCES_DIR),$(call recurse,f,$(dir),*.pica))

O_FILES 		:= $(patsubst $(SOURCES_DIR)/%,$(BUILD_DIR)/%,$(addsuffix .o, $(basename $(C_FILES) $(CXX_FILES))))
BIN_FILES		:= $(patsubst $(SOURCES_DIR)/%,$(BUILD_DIR)/%,$(addsuffix .bin, $(basename $(basename $(PICA_FILES)))))

INCLUDE			:= $(foreach dir,$(INCLUDE_DIR),-I$(CURDIR)/$(dir)) $(foreach dir,$(LIB_DIRS),-isystem $(dir)/include)
EMBED			:= $(foreach dir,$(EMBED_DIR),--embed-dir=$(CURDIR)/$(dir))

CC		:= arm-none-eabi-gcc
CXX		:= arm-none-eabi-g++
AR		:= arm-none-eabi-gcc-ar
LD		:= $(CXX)

# msys2 make and mingw32-make don't agree on PATH format on Windows
PATH := $(DEVKITPRO)/tools/bin/:$(DEVKITARM)/bin/:$(PATH)
PATH := $(subst C:/,/c/,$(DEVKITPRO)/tools/bin/:$(DEVKITARM)/bin/):$(PATH)

.PHONY: lib clean all

all: lib

$(BUILD_DIR)/%.bin: $(SOURCES_DIR)/%.v.pica
	$(info Compiling $^)
	@mkdir -p $(dir $@)
	@picasso -o $@ $^

$(BUILD_DIR)/%.bin: $(SOURCES_DIR)/%.v.pica $(SOURCES_DIR)/%.g.pica
	$(info Compiling $^)
	@mkdir -p $(dir $@)
	@picasso -o $@ $^

$(BUILD_DIR)/%.o: $(SOURCES_DIR)/%.c $(BIN_FILES)
	$(info Compiling $<)
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP -MF $(BUILD_DIR)/$*.d $(C_FLAGS) $(INCLUDE) $(EMBED) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCES_DIR)/%.cpp $(BIN_FILES)
	$(info Compiling $<)
	@mkdir -p $(dir $@)
	@$(CXX) -MMD -MP -MF $(BUILD_DIR)/$*.d $(CXX_FLAGS) $(INCLUDE) $(EMBED) -c $< -o $@

$(OUTPUT_DIR)/%.a: $(O_FILES)
	$(info Archiving $(notdir $@))
	@mkdir -p $(dir $@)
	@$(AR) rcs $@ $(O_FILES)

lib: $(OUTPUT_DIR)/lib$(TARGET).a
	@rm -rf $(OUTPUT_DIR)/$(TARGET)
	@cp -r $(INCLUDE_DIR)/* $(OUTPUT_DIR)/
	$(info Compiled $(TARGET)!)

clean:
	@echo Cleaning...
	@rm -rf $(BUILD_DIR) $(OUTPUT_DIR)

-include $(O_FILES:.o=.d)
