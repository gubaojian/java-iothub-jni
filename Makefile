DUKTAPE_SOURCES = duktape.c

JAVA_HOME = $(shell readlink -f /usr/bin/javac | sed "s:bin/javac::")include
JAVA_LINUX = $(JAVA_HOME)/linux
CC	= gcc
CCOPTS = -pedantic -std=c99 -Wall -fstrict-aliasing -shared -fpic
#CCOPTS	= -Os -pedantic -std=c99 -Wall -fstrict-aliasing -fomit-frame-pointer -shared -fpic
CCOPTS += -I$(JAVA_HOME) -I$(JAVA_LINUX)  # for combined sources
CCLIBS	= -lm
DEFINES =

all: libjsDuktapeJni.so

# For debugging, use -O0 -g -ggdb, and don't add -fomit-frame-pointer
libjsDuktapeJni.so: $(DUKTAPE_SOURCES) jsDuktapeWrapper.c
	$(CC) -o $@ $(DEFINES) $(CCOPTS) $(DUKTAPE_SOURCES) jsDuktapeWrapper.c $(CCLIBS)
	
