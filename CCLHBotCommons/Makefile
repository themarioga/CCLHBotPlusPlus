# Makefile for CCLHBot
# Created by Mario Gonzalez and Julian Garcia

# C compiler
CC = clang
# C++ compiler
CXX = clang++
# AR Linker
AR = ar
# Name of the executable created
TARGET = libCCLHBotCommons.a

####
# PATHS
####
# Path for the executable
BIN_PATH = ./lib
# Path for the .o files
BUILD_PATH = ./obj
# Paths for the .cpp files
SOURCE_PATHS = ./src
# Paths for the includes
INCLUDE_PATHS = /usr/include /usr/local/include /usr/include/tgbot /usr/include/jsoncpp ./include
# Paths for the libs
LIB_PATHS = /usr/lib

####
# FLAGS
####
# C flags
CFLAGS = -g -O3 -m64 -ffast-math
# Warning flags
CFLAGS += -Wall
# Preprocesor flags
CFLAGS += 
# C++ flags
CPPFLAGS = -std=c++14
# Linker flags
ARFLAGS = crf

######## DON'T EDIT ANYTHING BELOW THIS LINE
EXECUTABLE := $(BIN_PATH)/$(TARGET)

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SRC_DIRS := $(foreach DIR,$(SOURCE_PATHS),$(shell find $(DIR) -type d))
OBJ_DIRS := $(foreach DIR,$(SOURCE_PATHS),$(patsubst %, $(BUILD_PATH)/%,$(shell find $(DIR) -type d ! -path .)))

SRC_FILES := $(foreach DIR,$(SRC_DIRS),$(wildcard $(DIR)/*.cpp $(DIR)/*.c))
OBJ_FILES := $(foreach FILE,$(SRC_FILES),$(patsubst %.cpp, $(BUILD_PATH)/%.o, $(FILE)))

INCLUDE_DIRS := $(foreach DIR,$(INCLUDE_PATHS),$(patsubst %, -I%, $(DIR)))
CFLAGS += $(INCLUDE_DIRS)

LIBRARY_DIRS := $(foreach DIR,$(LIB_PATHS),$(patsubst %, -L%, $(DIR)))
LDFLAGS += $(LIBRARY_DIRS)

# MAKE OPTIONS
.PHONY: all run debug clean cleanfolder

all: $(BUILD_PATH) $(OBJ_FILES)
	$(info =================================)
	$(info Creando el ejecutable $(Target)...)
	$(info =================================)
	@$(AR) $(ARFLAGS) $(EXECUTABLE) $(OBJ_FILES)
    
$(BUILD_PATH)/%.o: %.c
	$(info Creando el binario para el archivo $<...)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_PATH)/%.o: %.cpp
	$(info Creando el binario para el archivo $<...)
	@$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_PATH):
	$(info =================================)
	$(info Creando la estructura de carpetas...)
	$(info =================================)
	@mkdir -p $(OBJ_DIRS)
	@mkdir -p $(BIN_PATH)

clean:
	$(info =================================)
	$(info Limpiando todo el proyecto...)
	$(info =================================)
	@$(RM) $(EXECUTABLE)
	@$(RM) -r $(OBJ_DIRS)
	@$(RM) -r $(BUILD_PATH)

cleanfolder:
	$(info =================================)
	$(info Limpiando la carpeta $(FOLDER)...)
	$(info =================================)
	@$(RM) $(EXECUTABLE)
	@$(RM) -r $(BUILD_PATH)/$(FOLDER)

install:
	$(info =================================)
	$(info Instalando la libreria $(FOLDER)...)
	$(info =================================)
	@mkdir -p /usr/local/include/CCLHBotCommons/
	@cp -R include/* /usr/local/include/CCLHBotCommons/
	@cp -R lib/* /usr/local/lib/
