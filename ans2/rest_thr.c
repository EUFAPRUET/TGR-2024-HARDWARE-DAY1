#include "iot_app.h"
#include <curl/curl.h>

const char base_url[] = "https://tnice-eea3c-default-rtdb.asia-southeast1.firebasedatabase.app/mem.json";

void *rest_thr_fcn(void *ptr) {
    (void) ptr;
    CURL *curl_handle;
    CURLcode res;
    char url[256];

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();

    if (curl_handle) {
        while (1) {
            // จำลองการส่งขนาดหน่วยความจำ
            int memory_size = shared_value;

            snprintf(url, sizeof(url), "%s", base_url);
            char json_data[100];
            snprintf(json_data, sizeof(json_data), "{\"mem\": %d}", memory_size);

            curl_easy_setopt(curl_handle, CURLOPT_URL, url);
            curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_data);

            res = curl_easy_perform(curl_handle);

            if (res != CURLE_OK) {
                fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(res));
            }
            printf("Sending memory data to Firebase: %d\n", memory_size);
            sleep(10); // ส่งข้อมูลทุก 10 วินาที
        }
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return NULL;
}
