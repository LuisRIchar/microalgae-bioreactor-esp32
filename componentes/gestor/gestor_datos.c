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
#include "gestor_datos.h"
#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static datos_turbidez_t turbidez;
static datos_lux_t lux_datos;
static datos_temp_t temp_datos;

float get_turbidez(void){

    xSemaphoreTake(turbidez.turbidez_mutex,portMAX_DELAY);
    float turb = turbidez.ntu;
    xSemaphoreGive(turbidez.turbidez_mutex);
    return turb;
}

void set_turbidez(float ntu){
    xSemaphoreTake(turbidez.turbidez_mutex,portMAX_DELAY);
    turbidez.ntu = ntu;
    xSemaphoreGive(turbidez.turbidez_mutex);
}

 float get_lux(void){
    xSemaphoreTake(lux_datos.lux_mutex,portMAX_DELAY);
    float luz = lux_datos.lux;
    xSemaphoreGive(lux_datos.lux_mutex);
    return luz;
 } 

 void set_lux(float lux){
    xSemaphoreTake(lux_datos.lux_mutex,portMAX_DELAY);
    lux_datos.lux = lux;
    xSemaphoreGive(lux_datos.lux_mutex);
 }

float get_temp(void){
    xSemaphoreTake(temp_datos.temp_mutex,portMAX_DELAY);
    float tem = temp_datos.val_temp;
    xSemaphoreGive(temp_datos.temp_mutex);
    return tem;
}

void set_temp(float temperatura_sensor){
    xSemaphoreTake(temp_datos.temp_mutex,portMAX_DELAY);
    temp_datos.val_temp = temperatura_sensor;
    xSemaphoreGive(temp_datos.temp_mutex);
}


void Init_gestor_datos(void){

    turbidez.turbidez_mutex = xSemaphoreCreateMutex();
    lux_datos.lux_mutex = xSemaphoreCreateMutex();
    temp_datos.temp_mutex =xSemaphoreCreateMutex();
}