a: devicedriver.o ftl.o main.o
	gcc -o $@ $^
devicedriver.o: devicedriver.c
	gcc -c $^
ftl.o: ftl.c
	gcc -c $^
main.o: main.c
	gcc -c $^
clean :
	rm devicedriver.o
	rm ftl.o
	rm main.o
	rm a
	rm flashmemory

