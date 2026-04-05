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

 #include "tira_led_roja.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "driver/ledc.h"
 #include "esp_log.h"

 #include "gestor_datos.h"

static const char* TAG = "Logica_led";

static tira_led_t led;

void logica_tira_led(void* pv){

    tira_led_t* tira = (tira_led_t*)pv;

    ledc_timer_config_t timer_cfg = {
        .timer_num = LEDC_TIMER_0,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .freq_hz = 5000,
        .duty_resolution = LEDC_TIMER_12_BIT,
    };
    ledc_timer_config(&timer_cfg);

    ledc_channel_config_t channel_cfg = {
        .gpio_num = tira->gpio_pwm,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&channel_cfg);

    float lux;

    while(1){

        lux = get_lux();

        //uint16_t normalizado =(uint16_t)  ((lux-0.0f)/(54600.0f-0.0f)*(4095));

        if(lux < 3000.0f){
            ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,3072);
            ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(3000));
        }else if(lux<12000){
            ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,2048);
            ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0);   
            vTaskDelay(pdMS_TO_TICKS(3000));
        }else if (lux <30000){
            ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,1024);
            ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(3000));
        }else{
            ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,100);
            ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        vTaskDelay(pdMS_TO_TICKS(300));

    }

}

void Init_tira_led(uint8_t pin){
    ESP_LOGI(TAG,"Logica de la tira led iniciada!");

    led.gpio_pwm = pin;

    xTaskCreatePinnedToCore(logica_tira_led,"led",4096,&led,3,NULL,1);
}
