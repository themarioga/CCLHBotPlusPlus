# Makefile for CCLHBot
# Created by Mario Gonzalez and Julian Garcia

# C compiler
CC = clang
# C++ compiler
CXX = clang++
# Debugger
DBG = lldb
# Name of the executable created
TARGET = jsontosql

####
# PATHS
####
# Path for the executable
BIN_PATH = ./bin
# Path for the .o files
BUILD_PATH = ./obj
# Paths for the .cpp files
SOURCE_PATHS = ./src
# Paths for the includes
INCLUDE_PATHS = /usr/include /usr/local/include /usr/include/jsoncpp
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
LDFLAGS = 
# Libraries
LIBS = -lboost_system -lssl -lcrypto -lpthread

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
	@$(CXX) $(OBJ_FILES) -o $(EXECUTABLE) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LIBS)
    
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

run: all
	$(info =================================)
	$(info Ejecutando...)
	$(info =================================)
	@$(EXECUTABLE) start cclh.db

debug: all
	$(info =================================)
	$(info Ejecutando...)
	$(info =================================)
	@$(DBG) $(EXECUTABLE)