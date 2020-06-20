########################################################################

CC      := gcc
EXE     :=
CCFLAGS := -g -O2
LDFLAGS  = $(CCFLAGS)
LIBS    := -lhidapi

ifeq ($(OS),Windows_NT)
EXE    := .exe
CCFLAGS := -mwindows $(CCFLAGS)
endif

OBJECT_FILES := main.o

%.o: %.c
	$(CC) -c -o $@ $(CCFLAGS) $<

./kbrgb$(EXE): $(OBJECT_FILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

kbrgb: ./kbrgb$(EXE)

.PHONY: all kbrgb
all: kbrgb

.DEFAULT_GOAL := all
