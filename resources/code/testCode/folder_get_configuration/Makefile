test_get_configuration: get_configuration.o test_get_configuration.o
	gcc -o test_get_configuration get_configuration.o test_get_configuration.o

test_get_configuration.o: get_configuration.h test_get_configuration.c
	gcc -c -o test_get_configuration.o test_get_configuration.c

get_configuration.o: common_header.h get_configuration.h get_configuration.c
	gcc -c -o get_configuration.o get_configuration.c

clean:	
	rm -f *.o test_get_configuration