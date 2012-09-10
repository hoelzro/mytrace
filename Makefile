mytrace: main.c syscall_info.o mysql_protocol.c
	gcc -o $@ $^

clean:
	rm -f mytrace *.o
