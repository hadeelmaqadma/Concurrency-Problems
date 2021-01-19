#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
//10 readers and 5 writers 
sem_t w;
pthread_mutex_t mutex;
int readers = 0;
int count =1 ;

void * writer(void * num){
    sem_wait(&w);
    count *=2 ;
    printf("Writer %d write the count as %d \n",(*((int *)num)), count);
    sem_post(&w);
    return 0 ; 
}

void * reader(void * num){
    // Reader acquire mutex lock
    pthread_mutex_lock(&mutex);
    readers++;
    if(readers == 1) {
        sem_wait(&w);
    }
    pthread_mutex_unlock(&mutex);
    // Reading Section
    printf("Reader %d: read count as %d\n",*((int *)num),count);

    // Reader acquire the lock before modifying readers
    pthread_mutex_lock(&mutex);
    readers--;
    if(readers == 0) {
        sem_post(&w); // If this is the last reader, it will wake up the writer.
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int main(){
   pthread_t read[10];
   pthread_t write[5];
   pthread_mutex_init(&mutex, NULL);
   sem_init(&w,0,1);
    int num[10] = {1,2,3,4,5,6,7,8,9,10};
    for(int i = 0; i < 10; i++) {
        pthread_create(&read[i], NULL, (void *)reader, (void *)&num[i]);
    }
    for(int i = 0; i <5; i++) {
        pthread_create(&write[i], NULL, (void *)writer, (void *)&num[i]);
    }
    for(int i = 0; i < 10; i++) {
        pthread_join(read[i], NULL);
    }
    for(int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&w);
     
     printf("The END\n");
    return 0;
}