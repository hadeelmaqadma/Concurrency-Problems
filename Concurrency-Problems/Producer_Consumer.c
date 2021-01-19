#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 10
int fill =0 ; 
int empty =0 ;

typedef struct {
    int buf[MAX]; 
    size_t size;
    pthread_mutex_t mutex; 
    pthread_cond_t empty; 
    pthread_cond_t fill; 
} buffer_t;

 void put(buffer_t *buffer, int value){
     buffer->buf[fill] = value;
     fill = (fill +1 ) %MAX; 
     ++buffer->size;
 }

 int get(buffer_t *buffer){
   int tmp=buffer->buf[empty];
   empty = (empty+1) %MAX; 
   --buffer->size;
   return tmp;
 }

void* producer(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;
    int i =0 ;
    while(1) {
        pthread_mutex_lock(&buffer->mutex);
        if(buffer->size == MAX) { // full
            pthread_cond_wait(&buffer->empty, &buffer->mutex);
        }
        i++;
        put(buffer , i);
        printf("Produced: %d\n", i);
        pthread_cond_signal(&buffer->fill);
        pthread_mutex_unlock(&buffer->mutex);
    }

 
    return 0;
}

void* consumer(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;
    while(1) {
        pthread_mutex_lock(&buffer->mutex);
        if(buffer->size == 0) { // empty
            pthread_cond_wait(&buffer->fill, &buffer->mutex);
        }

        printf("Consumed: %d\n", get(buffer));
        pthread_cond_signal(&buffer->empty);
        pthread_mutex_unlock(&buffer->mutex);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    buffer_t buffer = {
        .size = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .empty = PTHREAD_COND_INITIALIZER,
        .fill = PTHREAD_COND_INITIALIZER
    };
    
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, (void*)&buffer);
    pthread_create(&cons, NULL, consumer, (void*)&buffer);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}
