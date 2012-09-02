mytrace: main.c syscall_info.o
	gcc -o $@ $^

clean:
	rm -f mytrace *.o
