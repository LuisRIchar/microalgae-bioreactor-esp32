idf_component_register(
    SRCS "gestor_datos.c" "i2c_bus_compartido.c" "cliente_mqtt.c"
    INCLUDE_DIRS "."
    REQUIRES driver mqtt esp_wifi esp_event esp_netif nvs_flash
)