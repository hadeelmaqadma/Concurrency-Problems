#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#define MAX 10
int fill =0 ; 
int empty =0 ;

typedef struct {
    int buf[MAX]; 
    size_t size;
    pthread_mutex_t mutex; 
    sem_t empty; 
    sem_t fill; 
} buffer_t;

 void put(buffer_t *buffer, int value){
     buffer->buf[fill] = value;
     fill = (fill +1 ) %MAX;  
     ++buffer->size;
 }

 int get(buffer_t *buffer){
   int tmp=buffer->buf[empty];
   empty = (empty+1) %MAX; 
   empty +=1 ;
   --buffer->size;
   return tmp;
 }

void* producer(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;
    int i =0 ;
    while(1) {
        sem_wait(&buffer->fill);
        pthread_mutex_lock(&buffer->mutex);
        i++;
        put(buffer , i);
        printf("Produced: %d\n", i);
        pthread_mutex_unlock(&buffer->mutex);
        sem_post(&buffer->empty);   
    }
    return 0;
}

void* consumer(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;
    while(1) {
        sem_wait(&buffer->empty);
        pthread_mutex_lock(&buffer->mutex);
        printf("Consumed: %d\n", get(buffer));
        pthread_mutex_unlock(&buffer->mutex);
        sem_post(&buffer->fill);  
    }
    return 0;
}

int main(int argc, char *argv[]) {
    buffer_t buffer;
    buffer.size =0 ; 
    sem_init(&buffer.empty, 0, MAX) ;
    sem_init(&buffer.fill, 0, 0) ;
    pthread_t prod, cons;
    
    pthread_create(&prod, NULL, producer, (void*)&buffer);
    pthread_create(&cons, NULL, consumer, (void*)&buffer);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}