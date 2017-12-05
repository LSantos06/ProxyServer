default: compile

compile:
	gcc proxyServer_Filipe.c -o proxyServer.out -lpthread

clean:
	rm *.out

run:
	./proxyServer.out $(mode) $(port_number)

teste_inspecao:
	gcc inspecao.c ./test/teste_inspecao.c -o teste_inspecao.out && ./teste_inspecao.out
