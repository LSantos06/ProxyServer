default: compilar

compilar:
	gcc proxyServer.c -o proxy
	gcc caching.c -o caching -lpthread

rodar:
	./proxy 50000
