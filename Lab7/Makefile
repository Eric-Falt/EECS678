
# Change these lines before running make tar
FIRST_NAME=Eric
LAST_NAME=Faltermeier
KUID=3053092

LAB=09
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

CC = gcc
CFLAGS = -std=gnu11 -Wall -g

SRCDIR = ./src/libpriqueue
OBJDIR = ./obj

CFILE = $(SRCDIR)/libpriqueue.c
HFILE = $(SRCDIR)/libpriqueue.h
OFILE = $(OBJDIR)/libpriqueue.o

TEST_SRC = ./src/queuetest.c
TEST_EXEC = ./queuetest

$(TEST_EXEC): $(TEST_SRC) $(OFILE)
	$(CC) $(CFLAGS) $(TEST_SRC) $(OFILE) -o $(TEST_EXEC)

test: $(TEST_EXEC)
	$(TEST_EXEC)

# Compile the priority queue
all: $(OFILE)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OFILE): $(CFILE) $(HFILE) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $(CFILE) -o $(OFILE)

# Package the submission
tar: clean
	mkdir -p $(TAR_BASENAME)/src/libpriqueue
	cp $(CFILE) $(HFILE) $(TAR_BASENAME)/src/libpriqueue/
	cp Makefile $(TAR_BASENAME)/
	tar czvf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
	rm -rf $(TAR_BASENAME)

# Remove generated files
clean:
	rm -rf $(OBJDIR) $(TAR_BASENAME) $(TAR_BASENAME).tar.gz

.PHONY: all tar clean

