SHELL := /bin/bash
BIN := libsync.so

BUILD_DIR := build
SRC_DIR := src

INCLUDE := -I./include 
CFLAGS := -fPIC -shared -O3 #-DSPIN -DSPIN_TIMES=10 #-D_DEBUG
CXX := g++ 
CC := gcc

##############################################

SRC1 := $(wildcard $(SRC_DIR)/*.cpp)
SRC1 += $(wildcard $(SRC_DIR)/*.c)
SRC := $(subst $(SRC_DIR)/,,$(SRC1))
OBJ1 := $(patsubst %.cpp,%.o,${SRC1})
OBJ2 := $(patsubst %.c,%.o,${OBJ1})
OBJ := $(subst $(SRC_DIR)/,$(BUILD_DIR)/,$(OBJ2))
OBJ_S := $(subst $(SRC_DIR)/,,$(OBJ2))
DEP1 := $(patsubst %.cpp,%.d,${SRC1})
DEP2 := $(patsubst %.c,%.d,${DEP1})
DEP := $(subst $(SRC_DIR)/,$(BUILD_DIR)/,$(DEP2))

.PHONY: all clean test

vpath %.cpp $(SRC_DIR)
vpath %.c $(SRC_DIR)
vpath %.d $(BUILD_DIR)
vpath %.o $(BUILD_DIR)

all: $(OBJ_S)
	@if \
	$(CXX) $(OBJ) $(LIBS) $(CFLAGS) -o $(BIN);\
	then echo -e "[\e[32;1mLINK\e[m] \e[33m$(OBJ) $(DISASM_OBJ)\e[m \e[36m->\e[m \e[32;1m$(BIN)\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$(OBJ) $(DISASM_OBJ)\e[m \e[36m->\e[m \e[32;1m$(BIN)\e[m"; exit -1; fi;

###########################################
# DO NOT MODIFY THE FOLLOWING LINES
############################################

-include $(DEP) 

$(BUILD_DIR)/%.d: %.cpp
	@if [ ! -d $(BUILD_DIR) ]; then \
	mkdir $(BUILD_DIR); fi;
	@if \
	$(CXX) ${INCLUDE} -MM $< > $@;\
	then echo -e "[\e[32mCXX \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; exit -1; fi;

$(BUILD_DIR)/%.d: %.c
	@if [ ! -d $(BUILD_DIR) ]; then \
	mkdir $(BUILD_DIR); fi;
	@if \
	$(CC) ${INCLUDE} -MM $< > $@;\
	then echo -e "[\e[32mCC  \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; exit -1; fi;

%.o: %.cpp
	@if \
	$(CXX) ${CFLAGS} ${INCLUDE} -c $< -o $(BUILD_DIR)/$@; \
	then echo -e "[\e[32mCXX \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; exit -1; fi;

%.o: %.c
	@if \
	$(CC) ${CFLAGS} ${INCLUDE} -c $< -o $(BUILD_DIR)/$@; \
	then echo -e "[\e[32mCC  \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; exit -1; fi;

clean:
	@echo -e "[\e[32mCLEAN\e[m] \e[33m$(BIN) $(BUILD_DIR)\e[m"
	@rm -rf $(BIN) build 

