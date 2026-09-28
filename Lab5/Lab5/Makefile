STUDENT_ID=3053092
DIR=bash-4.2
NUM_FILES=10

build:
	gcc -Wall -g disk_usage.c -o disk_usage

disk_usage:
	/bin/bash disk_usage.sh $(DIR) $(NUM_FILES)

test:
	/bin/bash disk_usage.sh $(DIR) $(NUM_FILES) > tmp1
	./disk_usage $(DIR) $(NUM_FILES) > tmp2
	-diff tmp1 tmp2
	rm -f tmp1 tmp2

pipe:
	gcc -Wall -g pipe.c -o pipe

clean:
	rm -f disk_usage pipe tmp1 tmp2

tar:
	make clean
	mkdir $(STUDENT_ID)-ipc-lab
	cp -r bash-4.2 Makefile disk_usage.sh disk_usage.c $(STUDENT_ID)-ipc-lab
	tar cvzf $(STUDENT_ID)-ipc-lab.tar.gz $(STUDENT_ID)-ipc-lab
	rm -rf $(STUDENT_ID)-ipc-lab
