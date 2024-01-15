idf_component_register(SRCS "bmi2.c" "bmi270.c" "bmi270_context.c" "bmi270_maximum_fifo.c" "esp_common/bmi2_common.c"
                    INCLUDE_DIRS "." "./esp_common"
                    REQUIRES driver)