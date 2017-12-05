# ProxyServer
### Trabalho de Teleinformática e Redes 2 - Implementação de uma aplicação Proxy Server

### Componentes do Grupo 
* Filipe Teixeira Cardoso - 14/0139486
* Lucas Nascimento Santos Souza - 14/0151010
* Marcos Vinicius Prescendo Tonin - 14/0153233
* Victor Fabre Figueiredo - 15/0022948

### Instruções de Compilação e Execução
```shell
make:
  # compila as versoes do proxy com e sem cache

make clean:
  # remove os executaveis

make run_cache:
  # roda a versao do proxy com cache
  #   port_number=
  #   mode=
	./proxyServer_cache.out $(port_number) $(mode)

make run:
  # roda a versao do proxy sem cache
  #   port_number=
	./proxyServer.out $(port_number)
```
