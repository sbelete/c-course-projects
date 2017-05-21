#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "./race.h"

#define NFIB 40
#define BUF_SIZE 1024

int someone_finished = 0;

int main() {
    race_t *race = race_init();
    create_racers(race);
    start_race(race);
    return 0;
}

race_t *race_init() {
    race_t *race = (race_t *) malloc(sizeof(race_t));
    pthread_barrier_init(&(race->barrier), NULL, 4);
    pthread_mutex_init(&(race->mutex), NULL);
    return race;
}

int create_racers(race_t *race) {
    race->team_1 = create_team("Dory");
    race->team_2 = create_team("Nemo");
    return 0;
}

race_team_t *create_team(char *team_name) {
    race_team_t *team = (race_team_t *) malloc(sizeof(race_team_t));
    team->racer_a = create_racer(team_name);
    team->racer_b = create_racer(team_name);
    return team;
}

racer_t *create_racer(char *team_name) {
    racer_t *racer = (racer_t *) malloc(sizeof(racer_t));
    racer->finished = 0;
    racer->team_name = team_name;

    /**
     * TODO:
     * 1. initialize the racer's associated mutex and condition variable
     */
     pthread_mutex_init(&(racer->mutex), NULL);
     pthread_cond_init(&(racer->done), NULL);
    return racer;
}

void destroy_race(race_t *race) {
    /**
      * TODO: destroy the race barrier, race mutex, and all racer condition variables/mutexes
      */
	
    free(race->team_1->racer_a);
    free(race->team_1->racer_b);
    free(race->team_2->racer_a);
    free(race->team_2->racer_b);

    free(race->team_1);
    free(race->team_2);

    free(race);
}

void *run_racer_a(void *args) {
    /**
     * TODO:
     * 1. wait at barrier
     * 2. call calculate()
     * 3. set finished to 1
     * 4. signal condition variable
     */
     race_team_t *team = ((args_t *) args)->team;
    race_t *race = ((args_t *) args)->race;
     pthread_barrier_wait(&(race->barrier));
     calculate();
     team->racer_a->finished = 1;
    pthread_cond_broadcast(&(team->racer_b->done));
    return 0;
}

void *run_racer_b(void *args) {
    /**
     * TODO:
     * 1. wait at barrier
     * 2. wait on condition variable
     * 3. call handoff()
     * 4. call calculate()
     * 5. lock race mutex, call announce, unlock race mutex
     */
     race_team_t *team = ((args_t *) args)->team;
    race_t *race = ((args_t *) args)->race;

    pthread_barrier_wait(&(race->barrier));
    pthread_cond_wait(&(team->racer_b->done), &(team->racer_b->mutex));
    handoff(team->racer_b->team_name);
    calculate();
    pthread_mutex_lock(&(race->mutex));
    announce(team->racer_b->team_name);
    pthread_mutex_unlock(&(race->mutex));
    return 0;
}

void start_race(race_t *race) {
    /**
     * TODO:
     * 1. initialize the argument wrapper
     * 2. create racer threads
     * 3. start race!
     * 4. join threads
     */

    // all racers are done
     pthread_t threads[4];
     struct args_t* args[4];

    args[0] = (args_t *) malloc(sizeof(args_t));
    args[0]->race = race;
    args[0]->team = race->team_1;

    args[1] = (args_t *) malloc(sizeof(args_t));
    args[1]->race = race;
    args[1]->team = race->team_2;

    pthread_create(&threads[0], NULL, run_racer_a, args[0]);
    pthread_create(&threads[1], NULL, run_racer_b, args[0]);
    pthread_create(&threads[2], NULL, run_racer_a, args[1]);
    pthread_create(&threads[3], NULL, run_racer_b, args[1]);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);

    free(args[0]);
    free(args[1]);

    printf("All racers have finished\n");
    destroy_race(race);
}

void calculate() {
    fibonacci(NFIB);
}

int fibonacci(unsigned int a) {
    if (a < 2) {
        return 1;
    } else {
        return fibonacci(a - 1) + fibonacci(a - 2);
    }
}

void announce(char *team) {
    if (!someone_finished) {
        printf("Team %s has locked the Golden Mutex and won the race!\n", team);
    } else {
        printf("Team %s has come in second place, securing the Silver Mutex!\n",
            team);
    }

    someone_finished = 1;
}


/* printf is thread-safe, so racing threads shouldn't use it.
   this function is used so a team can announce when it
   has handed off without impeding progress of the other team. */
void *handoff_print(void *msg) {
    printf((char *)msg);
    free(msg);
    return 0;
}

void handoff(char* team) {
    pthread_t print_thread;
    char *buf = malloc(BUF_SIZE);
    snprintf(buf, BUF_SIZE, "Racer A of team %s has finished!\n", team);
    pthread_create(&print_thread, 0, handoff_print, buf);
}
