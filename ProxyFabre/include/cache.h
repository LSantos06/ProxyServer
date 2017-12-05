#define MAX_CACHE_SIZE (10<<20) /* 10MB */
#define MAX_OBJECT_SIZE (1<<20) /* 1MB */

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
