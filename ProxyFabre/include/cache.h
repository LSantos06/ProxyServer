#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CACHE_SIZE (1<<20) /* 10MB */
#define MAX_OBJECT_SIZE (1<<10) /* 1MB */

struct list_cache {
    char * data; /* HTML code */
	int size; /* size of the data */
	long int lru_time_track; /* least recently used */
	char * url;	/* identify cache objects by url */
	struct list_cache * next; 
};

typedef struct list_cache cache_object;

pthread_rwlock_t lock;

int add_to_cache(char * data, int site_size, char* url);
void remove_from_cache();
cache_object* cache_find(char * url);

/* Head of linked list cache_objects */
cache_object * head  ;
int cache_size;
long int global_time;