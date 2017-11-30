default: compile

compile:
	gcc proxyServer_Filipe.c -o proxyServer.out -lpthread

clean:
	rm *.out

run:
	./proxyServer.out $(mode) $(port_number)
