#ifndef IOT_APP_H
#define IOT_APP_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <cjson/cJSON.h>

// Shared variables
extern pthread_mutex_t data_cond_mutex;
extern pthread_cond_t data_cond;
extern int shared_value;

// Function prototypes
void *rest_thr_fcn(void *ptr);
void *mqtt_thr_fcn(void *ptr);
void init_db(const char *db_name);
void record_command(const char *command);

#endif // IOT_APP_H
