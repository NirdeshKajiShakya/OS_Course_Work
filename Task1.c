#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 3
#define TIME_QUANTUM 2

int shared_counter = 0;
int remaining_time[NUM_THREADS] = {5, 3, 4};

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_B = PTHREAD_MUTEX_INITIALIZER;

sem_t semaphores[NUM_THREADS];

void* worker_function(void* arg) {
    int id = *(int*)arg;
    
    // Loop until the thread has exhausted its required CPU time
    while (remaining_time[id] > 0) {
        sem_wait(&semaphores[id]); // Block until the scheduler signals this thread's turn
        
        int run_time = (remaining_time[id] < TIME_QUANTUM) ? remaining_time[id] : TIME_QUANTUM;
        sleep(run_time); // Simulate CPU execution
        remaining_time[id] -= run_time;
        
        // Strict lock ordering prevents circular wait deadlocks
        pthread_mutex_lock(&lock_A);
        pthread_mutex_lock(&lock_B);
        
        // Protect the shared counter from concurrent race conditions
        pthread_mutex_lock(&counter_mutex);
        shared_counter++;
        printf("Thread %d executed. Shared counter: %d\n", id, shared_counter);
        pthread_mutex_unlock(&counter_mutex);
        
        pthread_mutex_unlock(&lock_B);
        pthread_mutex_unlock(&lock_A);
        
        int next_id = (id + 1) % NUM_THREADS; // Round-robin handoff
        sem_post(&semaphores[next_id]);
    }
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {0, 1, 2};
    
    for (int i = 0; i < NUM_THREADS; i++) {
        sem_init(&semaphores[i], 0, 0); // Initialise all semaphores to 0 to block threads initially
    }
    sem_post(&semaphores[0]); // Kickstart the first thread in the round-robin chain
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker_function, &thread_ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        sem_destroy(&semaphores[i]);
    }
    
    printf("Final shared counter value: %d\n", shared_counter);
    return 0;
}