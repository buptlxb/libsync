SHELL := /bin/bash

BUILD_DIR := .
SRC_DIR := .
LIB_DIR := ../

INCLUDE := -I../include 
CFLAGS := 
CXXFLAGS := -std=c++11
LIBS := -lpthread -lsync -L${LIB_DIR} -Wl,-rpath,${LIB_DIR}
CXX := g++ 
CC := gcc

##############################################

SRC1 := $(wildcard $(SRC_DIR)/*.cpp)
SRC1 += $(wildcard $(SRC_DIR)/*.c)
SRC := $(subst $(SRC_DIR)/,,$(SRC1))
OBJ1 := $(patsubst %.cpp,%,${SRC1})
OBJ2 := $(patsubst %.c,%,${OBJ1})
OBJ := $(subst $(SRC_DIR)/,${BUILD_DIR}/,$(OBJ2))
OBJ_S := $(subst $(SRC_DIR)/,,$(OBJ2))

.PHONY: all clean test

vpath %.cpp $(SRC_DIR)
vpath %.c $(SRC_DIR)

all: $(OBJ_S)

###########################################
# DO NOT MODIFY THE FOLLOWING LINES
############################################

%: %.cpp
	@if \
	$(CXX) ${CXXFLAGS} ${INCLUDE} ${LIBS} $< -o ${BUILD_DIR}/$@; \
	then echo -e "[\e[32mCXX \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m${BUILD_DIR}/$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m${BUILD_DIR}/$@\e[m"; exit -1; fi;

%: %.c
	@if \
	$(CC) ${CFLAGS} ${INCLUDE} ${LIBS} $< -o ${BUILD_DIR}/$@; \
	then echo -e "[\e[32mCC  \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m${BUILD_DIR}/$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m${BUILD_DIR}/$@\e[m"; exit -1; fi;

clean:
	@echo -e "[\e[32mCLEAN\e[m] \e[33m ${OBJ_S}\e[m"
	@rm -rf ${OBJ_S} 

