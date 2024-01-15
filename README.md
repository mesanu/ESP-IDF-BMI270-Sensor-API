BMI270 Sensor API

This package contains the sensor APIs for the BMI270 sensor in addition to build files and integrations for use as a component in an ESP IDF generated project.

Please note that the examples have been removed, as has support for the legacy BMI270 API and support for the OIS interface. Please refer to the Bosch BMI270 API for for both.

This repo is a currently a work in a progress.

## Sensor Overview
The BMI270 is a small, low power, low noise inertial measurement unit designed for use in mobile applications like augmented reality or indoor navigation which require highly accurate, real-time sensor data.

## ESP IDF SPI bus set up
An example for the SPI bus set up is provided below.

**NOTE**
Do NOT configure command bits, dummy bits, or any other SPI transaction phases
other than the address phase. The BMI270 API does it's own handling of the read/write
command bits and dummy bits where necessary.

```
spi_device_interface_config_t SPIInterfaceConfig = {
    .address_bits = 8,
    .clock_speed_hz = {bus frequency},
    .mode = 0,
    .spics_io_num = {chip select pin},
    .queue_size = 1,
};
```

## Applications

### BMI270 (base)

- Any motion, No motion, Significant motion detectors
- Wrist worn Step counter and Step detector (Pedometer)
- Activity change recognition
  - Still
  - Walking
  - Running
- Wrist gestures
  - Push arm down
  - Pivot up
  - Wrist shake jiggle
  - Flick in
  - Flick out
- Wrist wear wake up

### BMI270 Context

- Step counter and Step detector (Pedometer)
- Activity change recognition
  - Still
  - Walking
  - Running

### BMI270 Maximum FIFO

- Supports a 6kB FIFO

For more information refer product page [Link](https://www.bosch-sensortec.com/products/motion-sensors/imus/bmi270.html) 

---