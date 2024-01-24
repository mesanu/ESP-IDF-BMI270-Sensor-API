/**
 * Copyright (C) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "bmi2_common.h"
#include "bmi2_defs.h"

#include "driver/spi_master.h"
#include "rom/ets_sys.h"

#include "esp_log.h"

/******************************************************************************/
/*!                 Macro definitions                                         */
#define BMI2_WRITE_COMMAND 0
#define BMI2_READ_COMMAND 1
#define BMI2_INTF_RET_FAIL 1

/******************************************************************************/
/*!                Static variable definition                                 */
static const char *TAG = "BMI270";

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * I2C read function map to ESP I2C API
 */
BMI2_INTF_RETURN_TYPE bmi2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    ESP_LOGE(TAG, "Error: I2C read attempt. I2C is currently not supported");
    return BMI2_INTF_RET_FAIL;
}

/*!
 * I2C write function map to ESP I2C API
 */
BMI2_INTF_RETURN_TYPE bmi2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    ESP_LOGE(TAG, "Error: I2C write attempt. I2C is currently not supported");
    return BMI2_INTF_RET_FAIL;
}

/*!
 * SPI read function map to ESP SPI API
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    bmi2_intf_config_t *interfaceConfig = (bmi2_intf_config_t *)intf_ptr;
    esp_err_t err = ESP_OK;
    spi_transaction_t transaction = {
        .addr = reg_addr,
        /* len is given in bytes while struct members are in bits */
        .length = len*8,
        .rxlength = len*8,
        .rx_buffer = reg_data,
    };
    if(xSemaphoreTake(interfaceConfig->spiSemaphore, 0)) {
        err = spi_device_polling_transmit(interfaceConfig->spiHandle, &transaction);
        xSemaphoreGive(interfaceConfig->spiSemaphore);
    } else {
        return BMI2_INTF_RET_FAIL;
    }
    
    if(err != ESP_OK) {
        return BMI2_INTF_RET_FAIL;
    }
    return BMI2_INTF_RET_SUCCESS;
}

/*!
 * SPI write function map to ESP SPI API
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{    
    bmi2_intf_config_t *interfaceConfig = (bmi2_intf_config_t *)intf_ptr;
    esp_err_t err = ESP_OK;
    spi_transaction_t transaction = {
        .addr = reg_addr,
        /* len is given in bytes while struct members are in bits */
        .length = len*8,
        .tx_buffer = reg_data,
    };
    
    if(xSemaphoreTake(interfaceConfig->spiSemaphore, 0)) {
        err = spi_device_polling_transmit(interfaceConfig->spiHandle, &transaction);
        xSemaphoreGive(interfaceConfig->spiSemaphore);
    } else {
        return BMI2_INTF_RET_FAIL;
    }
    if(err != ESP_OK) {
        return BMI2_INTF_RET_FAIL;
    }
    return BMI2_INTF_RET_SUCCESS;
}

/*!
 * Delay function map to ESP IDF API
 */
void bmi2_delay_us(uint32_t period, void *intf_ptr)
{
    ets_delay_us(period);
}

/*!
 *  @brief Function to select the interface between SPI and I2C.
 */
int8_t bmi2_interface_init(struct bmi2_dev *bmi, uint8_t intf)
{
    bmi2_intf_config_t *interfaceConfig = (bmi2_intf_config_t *)bmi->intf_ptr;
    esp_err_t err = ESP_OK;
    int8_t rslt = BMI2_OK;

    if (bmi != NULL)
    {

        /* Bus configuration : I2C */
        if (intf == BMI2_I2C_INTF) {
            rslt = BMI2_E_COM_FAIL;
        }
        /* Bus configuration : SPI */
        else if (intf == BMI2_SPI_INTF) {
            bmi->intf = BMI2_SPI_INTF;
            bmi->read = bmi2_spi_read;
            bmi->write = bmi2_spi_write;

            /* Configure delay in microseconds */
            bmi->delay_us = bmi2_delay_us;
            err = spi_bus_add_device(interfaceConfig->spiHost, &interfaceConfig->spiInterfaceConfig, &interfaceConfig->spiHandle);
            if(err != ESP_OK) {
                rslt = BMI2_E_COM_FAIL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }
    bmi2_error_codes_print_result(rslt);
    return rslt;
}


/*!
 *  @brief Prints the execution status of the APIs.
 */
void bmi2_error_codes_print_result(int8_t rslt)
{
    switch (rslt) {
        case BMI2_OK:
            /* Do nothing */
            break;
        case BMI2_W_FIFO_EMPTY:
            ESP_LOGW(TAG, "Warning [%d] : FIFO empty", rslt);
            break;
        case BMI2_W_PARTIAL_READ:
            ESP_LOGW(TAG, "Warning [%d] : FIFO partial read", rslt);
            break;
        case BMI2_E_NULL_PTR:
            ESP_LOGE(TAG, "Error [%d] : Null pointer error. It occurs when the user tries to assign value (not address) to a pointer, which has been initialized to NULL.", rslt);
            break;
        case BMI2_E_COM_FAIL:
            ESP_LOGE(TAG, "Error [%d] : Communication failure error. It occurs due to read/write operation failure and also due to power failure during communication", rslt);
            break;
        case BMI2_E_DEV_NOT_FOUND:
            ESP_LOGE(TAG, "Error [%d] : Device not found error. It occurs when the device chip id is incorrectly read", rslt);
            break;
        case BMI2_E_INVALID_SENSOR:
            ESP_LOGE(TAG, "Error [%d] : Invalid sensor error. It occurs when there is a mismatch in the requested feature with the available one", rslt);
            break;
        case BMI2_E_SELF_TEST_FAIL:
            ESP_LOGE(TAG, "Error [%d] : Self-test failed error. It occurs when the validation of accel self-test data is not satisfied", rslt);
            break;
        case BMI2_E_INVALID_INT_PIN:
            ESP_LOGE(TAG, "Error [%d] : Invalid interrupt pin error. It occurs when the user tries to configure interrupt pins apart from INT1 and INT2", rslt);
            break;
        case BMI2_E_OUT_OF_RANGE:
            ESP_LOGE(TAG, "Error [%d] : Out of range error. It occurs when the data exceeds from filtered or unfiltered data from fifo and also when the range exceeds the maximum range for accel and gyro while performing FOC", rslt);
            break;
        case BMI2_E_ACC_INVALID_CFG:
            ESP_LOGE(TAG, "Error [%d] : Invalid Accel configuration error. It occurs when there is an error in accel configuration register which could be one among range, BW or filter performance in reg address 0x40", rslt);
            break;
        case BMI2_E_GYRO_INVALID_CFG:
            ESP_LOGE(TAG, "Error [%d] : Invalid Gyro configuration error. It occurs when there is a error in gyro configuration register which could be one among range, BW or filter performance in reg address 0x42", rslt);
            break;
        case BMI2_E_ACC_GYR_INVALID_CFG:
            ESP_LOGE(TAG, "Error [%d] : Invalid Accel-Gyro configuration error. It occurs when there is a error in accel and gyro configuration registers which could be one among range, BW or filter performance in reg address 0x40 " "and 0x42", rslt);
            break;
        case BMI2_E_CONFIG_LOAD:
            ESP_LOGE(TAG, "Error [%d] : Configuration load error. It occurs when failure observed while loading the configuration into the sensor", rslt);
            break;
        case BMI2_E_INVALID_PAGE:
            ESP_LOGE(TAG, "Error [%d] : Invalid page error. It occurs due to failure in writing the correct feature configuration from selected page", rslt);
            break;
        case BMI2_E_SET_APS_FAIL:
            ESP_LOGE(TAG, "Error [%d] : APS failure error. It occurs due to failure in write of advance power mode configuration register", rslt);
            break;
        case BMI2_E_AUX_INVALID_CFG:
            ESP_LOGE(TAG, "Error [%d] : Invalid AUX configuration error. It occurs when the auxiliary interface settings are not enabled properly", rslt);
            break;
        case BMI2_E_AUX_BUSY:
            ESP_LOGE(TAG, "Error [%d] : AUX busy error. It occurs when the auxiliary interface buses are engaged while configuring the AUX", rslt);
            break;
        case BMI2_E_REMAP_ERROR:
            ESP_LOGE(TAG, "Error [%d] : Remap error. It occurs due to failure in assigning the remap axes data for all the axes after change in axis position", rslt);
            break;
        case BMI2_E_GYR_USER_GAIN_UPD_FAIL:
            ESP_LOGE(TAG, "Error [%d] : Gyro user gain update fail error. It occurs when the reading of user gain update status fails", rslt);
            break;
        case BMI2_E_SELF_TEST_NOT_DONE:
            ESP_LOGE(TAG, "Error [%d] : Self-test not done error. It occurs when the self-test process is ongoing or not completed", rslt);
            break;
        case BMI2_E_INVALID_INPUT:
            ESP_LOGE(TAG, "Error [%d] : Invalid input error. It occurs when the sensor input validity fails", rslt);
            break;
        case BMI2_E_INVALID_STATUS:
            ESP_LOGE(TAG, "Error [%d] : Invalid status error. It occurs when the feature/sensor validity fails", rslt);
            break;
        case BMI2_E_CRT_ERROR:
            ESP_LOGE(TAG, "Error [%d] : CRT error. It occurs when the CRT test has failed", rslt);
            break;
        case BMI2_E_ST_ALREADY_RUNNING:
            ESP_LOGE(TAG, "Error [%d] : Self-test already running error. It occurs when the self-test is already running and another has been initiated", rslt);
            break;
        case BMI2_E_CRT_READY_FOR_DL_FAIL_ABORT:
            ESP_LOGE(TAG, "Error [%d] : CRT ready for download fail abort error. It occurs when download in CRT fails due to wrong address location", rslt);
            break;
        case BMI2_E_DL_ERROR:
            ESP_LOGE(TAG, "Error [%d] : Download error. It occurs when write length exceeds that of the maximum burst length", rslt);
            break;
        case BMI2_E_PRECON_ERROR:
            ESP_LOGE(TAG, "Error [%d] : Pre-conditional error. It occurs when precondition to start the feature was not completed", rslt);
            break;
        case BMI2_E_ABORT_ERROR:
            ESP_LOGE(TAG, "Error [%d] : Abort error. It occurs when the device was shaken during CRT test", rslt);
            break;
        case BMI2_E_WRITE_CYCLE_ONGOING:
            ESP_LOGE(TAG, "Error [%d] : Write cycle ongoing error. It occurs when the write cycle is already running and another has been initiated", rslt);
            break;
        case BMI2_E_ST_NOT_RUNING:
            ESP_LOGE(TAG, "Error [%d] : Self-test is not running error. It occurs when self-test running is disabled while it's running", rslt);
            break;
        case BMI2_E_DATA_RDY_INT_FAILED:
            ESP_LOGE(TAG, "Error [%d] : Data ready interrupt error. It occurs when the sample count exceeds the FOC sample limit " "and data ready status is not updated", rslt);
            break;
        case BMI2_E_INVALID_FOC_POSITION:
            ESP_LOGE(TAG, "Error [%d] : Invalid FOC position error. It occurs when average FOC data is obtained for the wrong axes", rslt);
            break;
        default:
            ESP_LOGE(TAG, "Error [%d] : Unknown error code", rslt);
            break;
    }
}