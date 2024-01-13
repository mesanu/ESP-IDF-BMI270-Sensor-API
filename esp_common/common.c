/**
 * Copyright (C) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include "bmi2_defs.h"

/******************************************************************************/
/*!                 Macro definitions                                         */


/******************************************************************************/
/*!                Static variable definition                                 */

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * I2C read function map to ESP I2C API
 */
BMI2_INTF_RETURN_TYPE bmi2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return 0;
}

/*!
 * I2C write function map to ESP I2C API
 */
BMI2_INTF_RETURN_TYPE bmi2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return 0;
}

/*!
 * SPI read function map to ESP SPI API
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return 0;
}

/*!
 * SPI write function map to ESP SPI API
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return 0;
}

/*!
 * Delay function map to ESP IDF API
 */
void bmi2_delay_us(uint32_t period, void *intf_ptr)
{
}

/*!
 *  @brief Function to select the interface between SPI and I2C.
 */
int8_t bmi2_interface_init(struct bmi2_dev *bmi, uint8_t intf)
{
}


/*!
 *  @brief Prints the execution status of the APIs.
 */
void bmi2_error_codes_print_result(int8_t rslt)
{
}