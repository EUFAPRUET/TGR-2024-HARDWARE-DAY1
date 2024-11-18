#include "iot_app.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Shared variables
pthread_mutex_t data_cond_mutex;
pthread_cond_t data_cond;
int shared_value = 0;

int main(int argc, char *argv[]) {
    pthread_t rest_thr, mqtt_thr;

    if (argc < 3) {
        printf("Usage: %s REST_ENDPOINT MQTT_TOPIC\n", argv[0]);
        exit(1);
    }

    // Initialize mutex and condition variable
    pthread_mutex_init(&data_cond_mutex, NULL);
    pthread_cond_init(&data_cond, NULL);

    // Initialize database
    init_db("cmd.db");

    // Create REST and MQTT threads
    pthread_create(&rest_thr, NULL, rest_thr_fcn, (void*)argv[1]);
    pthread_create(&mqtt_thr, NULL, mqtt_thr_fcn, (void*)argv[2]);

    // Wait for threads to finish
    pthread_join(rest_thr, NULL);
    pthread_join(mqtt_thr, NULL);

    // Clean up
    pthread_mutex_destroy(&data_cond_mutex);
    pthread_cond_destroy(&data_cond);

    return 0;
}
