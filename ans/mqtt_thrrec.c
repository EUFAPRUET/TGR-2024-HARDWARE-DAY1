#include "iot_app.h"
#include <string.h>
#include <MQTTClient.h>
#include <stdlib.h>
#include <stdio.h>
#include <cjson/cJSON.h>

// Private constants
const char base_topic[] = "tgr2024/team/%s";
const char MQTT_BROKER[] = "tcp://broker.emqx.io:1883";
const char MQTT_CLIENTID[] = "TGR2024_Supachai";

// Shared variables
extern pthread_mutex_t data_cond_mutex;
extern pthread_cond_t data_cond;
extern int shared_value;

// Callback function เมื่อมีข้อความใหม่เข้ามาจาก MQTT Broker
int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    printf("Message arrived on topic: %s\n", topicName);
    char* payload = (char*)message->payload;

    // แปลง JSON ที่ได้รับ
    cJSON *json = cJSON_Parse(payload);
    if (json == NULL) {
        printf("Error parsing JSON\n");
    } else {
        // ดึงค่าจากคีย์ "value" ใน JSON
        cJSON *value_item = cJSON_GetObjectItem(json, "value");
        if (cJSON_IsNumber(value_item)) {
            int value = value_item->valueint;
            printf("Received value: %d\n", value);

            // อัปเดต shared_value
            pthread_mutex_lock(&data_cond_mutex);
            shared_value = value;
            pthread_cond_signal(&data_cond);
            pthread_mutex_unlock(&data_cond_mutex);
        } else {
            printf("Invalid JSON format\n");
        }
        cJSON_Delete(json);
    }

    // Cleanup
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// MQTT Thread function
void *mqtt_thr_fcn(void *ptr) {
    // Setup MQTT
    char topic[100];
    int rc;

    printf("Starting MQTT thread\n");
    MQTTClient mqtt_client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&mqtt_client, MQTT_BROKER, MQTT_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Connect to the MQTT broker
    if ((rc = MQTTClient_connect(mqtt_client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    // กำหนดหัวข้อ MQTT
    sprintf(topic, base_topic, (char*)ptr);
    printf("MQTT TOPIC: %s\n", topic);

    // ตั้งค่า callback function สำหรับการรับข้อความ
    MQTTClient_setCallbacks(mqtt_client, NULL, NULL, messageArrived, NULL);

    // Subscribe to the topic
    if ((rc = MQTTClient_subscribe(mqtt_client, topic, 0)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        MQTTClient_disconnect(mqtt_client, 10000);
        MQTTClient_destroy(&mqtt_client);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // คำสั่งอื่น ๆ ที่ต้องการรันในลูป (เช่น การเผยแพร่ข้อมูลตามเงื่อนไข)
        sleep(1); // พักให้โปรแกรมรันต่อเนื่อง
    }

    // Cleanup
    MQTTClient_disconnect(mqtt_client, 10000);
    MQTTClient_destroy(&mqtt_client);
    return NULL;
}
