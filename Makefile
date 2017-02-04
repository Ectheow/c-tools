tests = test_files
objs = test_files.o files.o test.o
link_objs = files.o test.o
CC=gcc

all: $(tests)

%.o: %.c
	$(CC) -c -o $@ $<

test: $(objs)
	for t in $(tests); do \
		$(CC) -o $$t $$t.o $(link_objs); \
		valgrind ./$$t;   \
	done
