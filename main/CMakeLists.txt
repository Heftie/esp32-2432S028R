idf_component_register(SRCS demo.c lcd.c touch.c
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_ledc
                    )
target_link_libraries(${COMPONENT_LIB} "-u _printf_float")