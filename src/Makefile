default: compile

compile:
	gcc cache.c estruturas.c filtragem.c proxy_cache.c -o proxyServer_cache.out -lpthread
	gcc filtragem.c proxy.c -o proxyServer.out -lpthread

clean:
	rm *.out

run_cache:
	./proxyServer_cache.out $(port_number) $(mode)

run:
	./proxyServer.out $(port_number)
