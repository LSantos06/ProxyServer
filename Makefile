default: compile

compile:
	gcc caching.c -o caching.out -lpthread

clean:
	rm *.out

run:
	./caching.out $(mode) $(port_number)
