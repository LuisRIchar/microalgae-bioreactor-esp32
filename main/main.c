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

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"

//librerias externas
#include "pantalla_oled.h"
#include "turbidez.h"
#include "gestor_datos.h"
#include "bh1750_luz.h"
#include "i2c_bus_compartido.h"
#include "ds18b20_temp.h"
#include "calentador_relay.h"
#include "tira_led_roja.h"
#include "cliente_mqtt.h"

void app_main(void){

    Init_gestor_datos();
    i2c_bus_init(11, 12);  
    Init_cliente_mqtt("SSID","PASSWORD","MQTT_URL");

    Init_pantalla(11,12,5);
    Init_sensor_luz(11,12);
    Init_sensor_turbidez(ADC1_CHANNEL_1);
    Init_sensor_temp(6);
    Init_relay_calentador(7);
    Init_tira_led(13);

    while (1){
        vTaskDelay(portMAX_DELAY);
    }
    
}
