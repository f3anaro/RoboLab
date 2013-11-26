# Makefile for the token search programm
# 
# 

# Target specific macros
TARGET = main
TARGET_SOURCES = $(addprefix src/, \
	movement.c \
	routing.c \
	display.c \
	melody.c \
	main.c )
TOPPERS_OSEK_OIL_SOURCE = ./main.oil

# Don't modify below part
O_PATH ?= build

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	INC=/Applications/robolab/nxtOSEK/toppers_osek/kernel
	include /Applications/robolab/nxtOSEK/ecrobot/ecrobot.mak
else
	INC=/cygdrive/C/cygwin/nxtOSEK/toppers_osek/kernel
	include /cygdrive/C/cygwin/nxtOSEK/ecrobot/ecrobot.mak
endif



