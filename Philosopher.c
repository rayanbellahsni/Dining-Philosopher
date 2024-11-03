// Rayan Bellahsni
// Dining Philosopher made in C/C++ using MultiThreading
// Method used is Two-Phase Procedure

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define MAX_HUNGER 100

// Structure for philosopher data
typedef struct {
    int id;
    int hunger;
    pthread_mutex_t *left_chopstick;
    pthread_mutex_t *right_chopstick;
} Philosopher;

pthread_mutex_t chopsticks[NUM_PHILOSOPHERS];
Philosopher philosophers[NUM_PHILOSOPHERS];
int running = 1;

// Function to simulate thinking and increase hunger
void think(Philosopher *philosopher) {
    philosopher->hunger += 10;
    printf("Philosopher %d is thinking. Hunger level: %d%%\n", philosopher->id, philosopher->hunger);
    sleep(1);

    if (philosopher->hunger >= MAX_HUNGER) {
        printf("Philosopher %d died of hunger! Game Over.\n", philosopher->id);
        running = 0;  // Stop the program when philosopher dies
    }
}

// Function to simulate eating and decreasing hunger
void eat(Philosopher *philosopher) {
    philosopher->hunger -= 30;
    if (philosopher->hunger < 0) philosopher->hunger = 0;

    printf("Philosopher %d is eating. Hunger level: %d%%\n", philosopher->id, philosopher->hunger);
    sleep(2);  // Time spent eating
}

// Function for each philosopher thread
void *philosopher_lifecycle(void *arg) {
    Philosopher *philosopher = (Philosopher *)arg;

    while (running) {
        // Philosopher is thinking
        think(philosopher);

        // Try to pick up left chopstick first, then right (Two-Phase Procedure)
        if (pthread_mutex_lock(philosopher->left_chopstick) == 0) {
            if (pthread_mutex_lock(philosopher->right_chopstick) == 0) {
                // Philosopher is eating
                eat(philosopher);

                // Put down right chopstick
                pthread_mutex_unlock(philosopher->right_chopstick);
            }
            // Put down left chopstick
            pthread_mutex_unlock(philosopher->left_chopstick);
        }

        // Small delay to avoid deadlock and allow other philosophers to act
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_PHILOSOPHERS];
   
    // Initialize philosophers and chopsticks
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i].id = i;
        philosophers[i].hunger = 0;
        philosophers[i].left_chopstick = &chopsticks[i];
        philosophers[i].right_chopstick = &chopsticks[(i + 1) % NUM_PHILOSOPHERS];
        pthread_mutex_init(&chopsticks[i], NULL);
    }

    // Create threads for each philosopher
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&threads[i], NULL, philosopher_lifecycle, &philosophers[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup memory
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&chopsticks[i]);
    }

    return 0;
}

