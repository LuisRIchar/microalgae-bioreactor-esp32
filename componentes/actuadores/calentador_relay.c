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

#include "calentador_relay.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdbool.h>

#include "gestor_datos.h"

static const char* TAG = "Relay_calentador";

static relay_pin_t pin_relay;

void logica_relay(void* pv) {
    relay_pin_t* relay = (relay_pin_t*)pv;

    gpio_config_t relay_cfg = {
        .pin_bit_mask = (1ULL << relay->gpio_relay),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&relay_cfg);

    // Relay invertido: 0 = encendido, 1 = apagado
    const float TEMP_ON  = 23.5f;   // Enciende por debajo de este valor
    const float TEMP_OFF = 24.5f;   // Apaga por encima de este valor
    const TickType_t MIN_SWITCH_INTERVAL = pdMS_TO_TICKS(30000); // 30 s

    bool calentador_encendido = false;
    TickType_t ultimo_cambio = xTaskGetTickCount();

    // Estado inicial: apagado
    gpio_set_level(relay->gpio_relay, 1);

    while (1) {
        float temperatura = get_temp();
        TickType_t ahora = xTaskGetTickCount();

        bool estado_deseado = calentador_encendido;

        // Histéresis
        if (!calentador_encendido && temperatura <= TEMP_ON) {
            estado_deseado = true;
        } else if (calentador_encendido && temperatura >= TEMP_OFF) {
            estado_deseado = false;
        }

        // Tiempo minimo entre conmutaciones
        if (estado_deseado != calentador_encendido &&
            (ahora - ultimo_cambio) >= MIN_SWITCH_INTERVAL) {

            calentador_encendido = estado_deseado;
            ultimo_cambio = ahora;

            gpio_set_level(relay->gpio_relay, calentador_encendido ? 0 : 1);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Init_relay_calentador(gpio_num_t num) {
    ESP_LOGI(TAG, "logica relay inicializado");

    pin_relay.gpio_relay = num;

    xTaskCreatePinnedToCore(logica_relay, "relay", 4096, &pin_relay, 3, NULL, 1);
}
