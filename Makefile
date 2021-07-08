# -*- Makefile -*-

########################
# CONFIGURABLE OPTIONS #
########################

CC     := clang++ 
CFLAGS := -O3 -std=c++20 -Wall
CLIBS  := -lsfml-window -lsfml-graphics -lsfml-system -pthread

TARGET := main.cpp

SRC_FOLDER := src/
BIN_FOLDER := bin/

#################
# DO NOT MODIFY #
#################

_OUT := $(BIN_FOLDER)$(basename $(TARGET))
_FILES = $(basename $(notdir $(shell find $(SRC_FOLDER) -name "*.cpp")))
_PREFIX := "%>"

###########
# RECIPES #
###########

all: compile target
	@echo $(_PREFIX) Compiled project

target:
	@echo $(_PREFIX) Compiling \"$(TARGET)\" to \"$(_OUT)\"
	@$(CC) $(CFLAGS) $(CLIBS) $(TARGET) $(BIN_FOLDER)/* -o $(_OUT)

compile:
	@echo $(_PREFIX) Compiling source files from \"$(SRC_FOLDER)\" to \"$(BIN_FOLDER)\"
	@for file in $(_FILES); do \
		$(CC) $(CFLAGS) -c $(SRC_FOLDER)$$file.cpp -o $(BIN_FOLDER)$$file.o; \
	done

clean:
	@echo $(_PREFIX) Removing compiled files
	@rm $(_OUT)
	@rm -rf bin/*

run:
	@echo $(_PREFIX) Running $(_OUT)
	./$(_OUT)

print-%: 
	@echo $($*)%  