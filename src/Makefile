OBJ_FILES = $(DST)/lexer.o $(DST)/main.o $(DST)/util.o $(DST)/AST.o $(DST)/parser.o
EXEC = run
DST = ../dst
INCLUDE = ../include
CC = gcc
LD = gcc
CFLAGS = -Wall -std=c11 -c
BINDIR = ../bin

.PHONY: all
all: $(BINDIR) $(OBJ_FILES) $(HEADERS)
	$(LD) $(OBJ_FILES) -o $(BINDIR)/$(EXEC)

$(DST)/%.o: %.c
	$(CC) $*.c $(CFLAGS) -I $(INCLUDE) -o $(DST)/$*.o
