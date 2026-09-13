STUDENT_ID=314156

# Define the compiler
CC = gcc

# Define any compile-time flags
CFLAGS = -Wall -g

# Define any directories containing header files
INCLUDES = -I. -ITests

# Define library paths in addition to /usr/lib
LFLAGS =

# Define any libraries to link into executable
LIBS =

# Define the C source files
SRCS = main.c

# Define the C header files (included in the compilation by the .c files)
HEADERS = LinkedList.h ListQuestions.h Tests/test1.h

# Define the executable file
MAIN = runTests

.PHONY: clean

all: $(MAIN) bugs valgrind_test
	@echo Program has been compiled!

$(MAIN): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(SRCS) $(LFLAGS) $(LIBS)

bugs: bugs.c
	$(CC) -g $^ -o $@

valgrind_test: valgrind_test.c
	$(CC) -g $^ -o $@

test: bugs
	-valgrind --leak-check=full --track-origins=yes ./bugs > bugs_output.txt 2> valgrind_output.txt
	@sed -i 's/^==[0-9]*==/==#==/' valgrind_output.txt
	@echo ""
	@echo "======================================="
	@echo "VALGRIND DIFF"
	-@diff -a valgrind_desired.txt valgrind_output.txt
	@echo "---------------------------------------"
	@echo ""
	@echo "======================================="
	@echo "PROGRAM DIFF"
	-@diff -a bugs_desired.txt bugs_output.txt

clean:
	-rm bugs valgrind_test
	$(RM) *.o *~ $(MAIN)

tar: clean
	mkdir $(STUDENT_ID)_eecs678_lab1
	cp -r bugs.c Makefile main.c Tests LinkedList.h ListQuestions.h $(STUDENT_ID)_eecs678_lab1
	tar cfzv $(STUDENT_ID)_eecs678_lab1.tar.gz $(STUDENT_ID)_eecs678_lab1
	-rm -rf $(STUDENT_ID)_eecs678_lab1

.PHONY: clean test
