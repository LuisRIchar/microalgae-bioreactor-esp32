/*
 * Copyright 2026 Luis Ricardo Serrano Dzib
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 #include "cliente_mqtt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "gestor_datos.h"

static const char* TAG="Cleinte_mqtt";

static red_conexion_t red;
static esp_mqtt_client_handle_t cliente_mqtt = NULL;

static void wifi_handler(void* args,esp_event_base_t evento, int32_t id_evento,void* datos_evento){
    if(evento == WIFI_EVENT && id_evento == WIFI_EVENT_STA_START){
        esp_wifi_connect();
    }else if(evento ==WIFI_EVENT&& id_evento == WIFI_EVENT_STA_DISCONNECTED){
        ESP_LOGW(TAG,"Sin wifi, tratando de reconectar");
        esp_wifi_connect();
    }else if(evento == IP_EVENT && id_evento == IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t* ip_evento =(ip_event_got_ip_t*)datos_evento;
        ESP_LOGI(TAG,"Ip obtenida: " IPSTR,IP2STR(&ip_evento->ip_info.ip));

        if(cliente_mqtt != NULL){
            esp_mqtt_client_start(cliente_mqtt);
        }
    }
    

}

static void wifi_init_sta(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_handler, NULL, NULL);

    wifi_config_t wifi_config = {0};
    snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", red.SSID);
    snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", red.Contraseña);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    vTaskDelay(pdMS_TO_TICKS(5000));
}

void publicar_datos(void* pv){

    red_conexion_t* conexion = (red_conexion_t*)pv;

    float luz;
    float temperatura;
    float turbidez;
    char trama[60];

    while(1){
        vTaskDelay(pdMS_TO_TICKS(30000));

        luz = get_lux();
        temperatura = get_temp();
        turbidez = get_turbidez();

        snprintf(trama,sizeof(trama),"%.2f,%.2f,%.2f",luz,temperatura,turbidez);

        if(cliente_mqtt != NULL){
            esp_mqtt_client_publish(cliente_mqtt,"bioreactor/sensores",trama,0,1,0);
            ESP_LOGI(TAG,"datos enviados al servidor mqtt: %.2f luz,%.2f temperatura,%.2f turbidez",luz,temperatura,turbidez);
        }

    }

}

void Init_cliente_mqtt(const char *ssid, const char *contra, const char *broker){
    ESP_LOGI(TAG,"Iniciando cliente mqtt");

    snprintf(red.SSID,sizeof(red.SSID),"%s",ssid);
    snprintf(red.Contraseña,sizeof(red.Contraseña),"%s",contra);
    snprintf(red.broker,sizeof(red.broker),"%s",broker);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    
    
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = red.broker,
    };
    cliente_mqtt = esp_mqtt_client_init(&mqtt_cfg);

    wifi_init_sta();
    //esp_mqtt_client_start(cliente_mqtt);

    xTaskCreatePinnedToCore(publicar_datos,"mqtt_cleinte",4096,&red,3,NULL,0);
}
