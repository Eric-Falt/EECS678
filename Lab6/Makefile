# Add the dp_asymmetric and dp_waiter targets to all as you implement
# them

STUDENT_ID=3053092

SRCDIR = ./
CFILELIST = dp_asymmetric.c dp_waiter.c ptcount_mutex.c ptcount_atomic.c

RAWC = $(patsubst %.c,%,$(addprefix $(SRCDIR), $(CFILELIST)))

LOOP=100000000
LOOP_HELGRIND=1
INC=1

all: dp_asymmetric dp_waiter ptcount_mutex ptcount_atomic

ptcount_mutex: ptcount_mutex.c
	gcc $(CCFLAGS) -g -o $@ $^ -lpthread

ptcount_atomic: ptcount_atomic.c
	gcc $(CCFLAGS) -g -o $@ $^ -lpthread

pt_all: ptcount_mutex ptcount_atomic

pt_test: pt_all
	time ./ptcount_mutex $(LOOP) $(INC)
	time ./ptcount_atomic $(LOOP) $(INC)
	

test-helgrind: all
	valgrind --tool=helgrind ./ptcount_mutex $(LOOP_HELGRIND) $(INC)
	valgrind --tool=helgrind ./ptcount_atomic $(LOOP_HELGRIND) $(INC)


dp: dining_philosophers.c
	gcc -g dining_philosophers.c -lpthread -lm -o dp

dp_asymmetric: dp_asymmetric.c
	gcc -g dp_asymmetric.c -lpthread -lm -o dp_asymmetric

dp_waiter: dp_waiter.c
	gcc -g dp_waiter.c -lpthread -lm -o dp_waiter

# Add the dp_asymmetric_test and dp_waiter_test targets to test as you implement
# them

dp_test: dp_asymmetric dp_waiter
	./dp_asymmetric
	./dp_waiter

dp_asymmetric_test: dp_asymmetric
	./dp_asymmetric

dp_waiter_test: dp_waiter
	./dp_waiter

clean:
	rm -f dp dp_asymmetric dp_waiter
	rm -f ptcount_mutex ptcount_atomic
	rm -rf *-c.txt $(STUDENT_ID)-pthreads_dp-lab

zip: 
	make clean
	mkdir $(STUDENT_ID)-pthreads_intro-lab
#	get all the c files to be .txt for archiving	
	$(foreach file, $(RAWC), cp $(file).c $(file)-c.txt;)
#	copy files into temp folder
	cp Makefile $(CFILELIST) $(STUDENT_ID)-pthreads_intro-lab/
	mv *-c.txt $(STUDENT_ID)-pthreads_intro-lab/
	zip -r $(STUDENT_ID)-pthreads_intro-lab.zip $(STUDENT_ID)-pthreads_intro-lab
	rm -rf $(STUDENT_ID)-pthreads_intro-lab
tar:
	make clean
	mkdir $(STUDENT_ID)-pthreads_intro-lab
	# Get all the C files as .txt for archiving
	$(foreach file, $(RAWC), cp $(file).c $(file)-c.txt;)
	# Copy files into temp folder
	cp Makefile $(CFILELIST) $(STUDENT_ID)-pthreads_intro-lab/
	# Create tar.gz archive
	tar -czf $(STUDENT_ID)-pthreads_intro-lab.tar.gz $(STUDENT_ID)-pthreads_intro-lab
	# Remove temporary directory
	rm -rf $(STUDENT_ID)-pthreads_intro-lab
