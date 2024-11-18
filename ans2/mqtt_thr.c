#include "iot_app.h"
#include <string.h>
#include <MQTTClient.h>
#include <cjson/cJSON.h>

const char MQTT_BROKER[] = "tcp://broker.emqx.io:1883";
const char MQTT_CLIENTID[] = "RaspberryPi_Client";
const char SUBSCRIBE_TOPIC[] = "device/command";

void *mqtt_thr_fcn(void *ptr) {

    MQTTClient mqtt_client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClient_create(&mqtt_client, MQTT_BROKER, MQTT_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(mqtt_client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(&mqtt_client);
        return NULL;
    }

    MQTTClient_subscribe(mqtt_client, SUBSCRIBE_TOPIC, 0);

    while (1) {
        MQTTClient_message *message = NULL;
        char *topicName = NULL;
        int topicLen;

        if (MQTTClient_receive(mqtt_client, &topicName, &topicLen, &message, 1000) == MQTTCLIENT_SUCCESS && message) {
            char *payload = (char*) message->payload;
            printf("Received message: %s\n", payload);

            // Parse JSON command
            cJSON *json = cJSON_Parse(payload);
            if (json) {
                cJSON *check = cJSON_GetObjectItem(json, "check");
                if (cJSON_IsString(check)) {
                    if (strcmp(check->valuestring, "MemStatus") == 0) {
                        pthread_mutex_lock(&data_cond_mutex);
                        shared_value = 1024; // ตัวอย่างค่า จำลองขนาดหน่วยความจำ
                        pthread_mutex_unlock(&data_cond_mutex);
                        record_command("MemStatus");
                    }
                }
                cJSON_Delete(json);
            }

            MQTTClient_freeMessage(&message);
            MQTTClient_free(topicName);
        }
    }

    MQTTClient_disconnect(mqtt_client, 1000);
    MQTTClient_destroy(&mqtt_client);
   


    return NULL;
}
