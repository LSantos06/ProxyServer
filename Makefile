default: compilar

compilar:
	gcc proxyServer.c -o proxy

rodar:
	./proxy 50000