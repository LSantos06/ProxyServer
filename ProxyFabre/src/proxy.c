#include "filtragem.h"
#include "estruturas.h"
#include "cache.h"


int main(int argc, char *argv[]){
	
    // c_: about client-proxy interface (proxy is server)
    int c_serverFD = 0, c_opt = 1;
    int c_newSocketFD = 0, *c_pNewSocketFD = NULL;
    struct sockaddr_in c_serverAddr;
    int c_addrLen = sizeof(struct sockaddr_in);
    pthread_t v_thr[4];
    int aux_erro,i=0;
    global_time =0;
    cache_size = 0;

    if(argc < 3){
        perror("Para executar ./proxy <modo> <numero_da_porta> \n Lembrando que eh porta utilizada pelo IP do LoopBack");
        exit(EXIT_FAILURE);
    }
    if(argc > 3)
    {
        perror("too many arguments\n");
        exit(EXIT_FAILURE);
    }

    if(!strcmp(argv[1],"-i")){
        printf("INSPECAO\n");
    }
    else if(!strcmp(argv[1],"-f")){
        printf("FILTRAGEM\n");
    }
    else if(!strcmp(argv[1],"-c")){
        printf("CACHE\n");
    }
    else{
        printf("invalid argument %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(atoi(argv[2]) <= 1024 || atoi(argv[2]) >= 65535)
    {
        perror("invalid port argument\n");
        exit(EXIT_FAILURE);
    }

    // Creating proxy server socket file descriptor
    if((c_serverFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) // == -1
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    puts("socket succeded");

    // Optional: helps in reuse of address and port, prevents error such as “address already in use”
    if(setsockopt(c_serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &c_opt, sizeof(c_opt)) < 0) // == -1
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    puts("setsockport succeded");

    // Preparing proxy server sockaddr_in structure
    c_serverAddr.sin_family = AF_INET;
    c_serverAddr.sin_addr.s_addr = INADDR_ANY; // inet_addr("127.0.0.1");
    c_serverAddr.sin_port = htons(atoi(argv[1]));
    
    // Biding c_serverFD to c_serverAddr
    if(bind(c_serverFD, (struct sockaddr *)&c_serverAddr, c_addrLen) < 0) // == -1
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    puts("bind succeded");

    // Start listening for incoming conections requests
    if(listen(c_serverFD, BACKLOG) < 0) // == -1
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    puts("listen succeded");

    // Waiting or accepting incoming connections
    puts("waiting for incoming connections...");
    
    while(i < 4){
        aux_erro = pthread_create(&v_thr[i],NULL,&connectionHandler,(void *) &c_serverFD);
        if(aux_erro < 0){
            puts("Failed do create thread");
        }    
    i++;
    }
    for(i =0;i<4;i++){
        pthread_join(v_thr[i],NULL);
    }
    puts("waiting for incoming connections...");


    return 0;
}
