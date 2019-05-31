all: main
main: entry.o sumodd.o wordswap.o findabsence.o
	gcc -w -o $@ $^
entry.o: entry.c
	gcc -w -c entry.c
sumodd.o: sumodd.c
	gcc -w -c sumodd.c
wordswap.o: wordswap.c
	gcc -w -c wordswap.c
findabsence.o: findabsence.c
	gcc -w -c findabsence.c
clean:
	@echo "Cleaning"
	rm -f *.o main
run: main
	./main
