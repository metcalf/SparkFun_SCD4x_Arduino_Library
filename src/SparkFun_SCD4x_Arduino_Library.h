/*
  This is a library written for the SCD4x family of CO2 sensors
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/nnnnn

  Written by Paul Clark @ SparkFun Electronics, June 2nd, 2021

  The SCD41 measures CO2 from 400ppm to 5000ppm with an accuracy of +/- 40ppm + 5% of reading

  This library handles the initialization of the SCD4x and outputs
  CO2 levels, relative humidty, and temperature.

  https://github.com/sparkfun/SparkFun_SCD4x_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.13

  SparkFun code, firmware, and software is released under the MIT License.
  Please see LICENSE.md for more details.
*/

#ifndef __SparkFun_SCD4x_ARDUINO_LIBARARY_H__
#define __SparkFun_SCD4x_ARDUINO_LIBARARY_H__

// Uncomment the next #define if using an Teensy >= 3 or Teensy LC and want to use the dedicated I2C-Library for it
// Then you also have to include <i2c_t3.h> on your application instead of <Wire.h>

// #define USE_TEENSY3_I2C_LIB

#include "Arduino.h"
#ifdef USE_TEENSY3_I2C_LIB
#include <i2c_t3.h>
#else
#include <Wire.h>
#endif

//The default I2C address for the SCD4x is 0x62.
#define SCD4x_ADDRESS 0x62

//Available commands

//Basic Commands
#define SCD4x_COMMAND_START_PERIODIC_MEASUREMENT              0x21b1
#define SCD4x_COMMAND_READ_MEASUREMENT                        0xec05 // execution time: 1ms
#define SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT               0x3f86 // execution time: 500ms

//On-chip output signal compensation
#define SCD4x_COMMAND_SET_TEMPERATURE_OFFSET                  0x241d // execution time: 1ms
#define SCD4x_COMMAND_GET_TEMPERATURE_OFFSET                  0x2318 // execution time: 1ms
#define SCD4x_COMMAND_SET_SENSOR_ALTITUDE                     0x2427 // execution time: 1ms
#define SCD4x_COMMAND_GET_SENSOR_ALTITUDE                     0x2322 // execution time: 1ms
#define SCD4x_COMMAND_SET_AMBIENT_PRESSURE                    0xe000 // execution time: 1ms

//Field calibration
#define SCD4x_COMMAND_PERFORM_FORCED_CALIBRATION              0x362f // execution time: 400ms
#define SCD4x_COMMAND_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2416 // execution time: 1ms
#define SCD4x_COMMAND_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2313 // execution time: 1ms

//Low power
#define SCD4x_COMMAND_START_LOW_POWER_PERIODIC_MEASUREMENT    0x21ac
#define SCD4x_COMMAND_GET_DATA_READY_STATUS                   0xe4b8 // execution time: 1ms

//Advanced features
#define SCD4x_COMMAND_PERSIST_SETTINGS                        0x3615 // execution time: 800ms
#define SCD4x_COMMAND_GET_SERIAL_NUMBER                       0x3682 // execution time: 1ms
#define SCD4x_COMMAND_PERFORM_SELF_TEST                       0x3639 // execution time: 10000ms
#define SCD4x_COMMAND_PERFORM_FACTORY_RESET                   0x3632 // execution time: 1200ms
#define SCD4x_COMMAND_REINIT                                  0x3646 // execution time: 20ms

//Low power single shot - SCD41 only
#define SCD4x_COMMAND_MEASURE_SINGLE_SHOT                     0x219d // execution time: 5000ms
#define SCD4x_COMMAND_MEASURE_SINGLE_SHOT_RHT_ONLY            0x2196 // execution time: 50ms


typedef union
{
  int16_t signed16;
  uint16_t unsigned16;
} scd4x_signedUnsigned16_t; // Avoid any ambiguity casting int16_t to uint16_t

typedef union
{
  uint16_t unsigned16;
  uint8_t bytes[2];
} scd4x_unsigned16Bytes_t; // Make it easy to convert 2 x uint8_t to uint16_t

typedef enum
{
  SCD4x_SENSOR_SCD40 = 0,
  SCD4x_SENSOR_SCD41
} scd4x_sensor_type_e;

class SCD4x
{
public:
  SCD4x(scd4x_sensor_type_e sensorType = SCD4x_SENSOR_SCD40);

  bool begin(bool measBegin) { return begin(Wire, measBegin); }
  bool begin(bool measBegin, bool autoCalibrate) { return begin(Wire, measBegin, autoCalibrate); }
  bool begin(bool measBegin, bool autoCalibrate, bool skipStopPeriodicMeasurements) { return begin(Wire, measBegin, autoCalibrate, skipStopPeriodicMeasurements); }
#ifdef USE_TEENSY3_I2C_LIB
  bool begin(i2c_t3 &wirePort = Wire, bool measBegin = true, bool autoCalibrate = true, bool skipStopPeriodicMeasurements = false); //By default use Wire port
#else
  bool begin(TwoWire &wirePort = Wire, bool measBegin = true, bool autoCalibrate = true, bool skipStopPeriodicMeasurements = false); //By default use Wire port
#endif

  void enableDebugging(Stream &debugPort = Serial); //Turn on debug printing. If user doesn't specify then Serial will be used.

  bool startPeriodicMeasurement(void); //Signal update interval is 5 seconds
  // stopPeriodicMeasurement can be called before .begin if required
  // If the sensor has been begun (_i2cPort is not NULL) then _i2cPort is used
  // If the sensor has not been begun (_i2cPort is NULL) then wirePort and address are used (which will default to Wire)
#ifdef USE_TEENSY3_I2C_LIB
  bool stopPeriodicMeasurement(uint16_t delayMillis = 500, i2c_t3 &wirePort = Wire); //Note that the sensor will only respond to other commands after waiting 500 ms after issuing the stop_periodic_measurement command.
#else
  bool stopPeriodicMeasurement(uint16_t delayMillis = 500, TwoWire &wirePort = Wire); //Note that the sensor will only respond to other commands after waiting 500 ms after issuing the stop_periodic_measurement command.
#endif

  bool readMeasurement(void);

  uint16_t getCO2(void);
  float getHumidity(void);
  float getTemperature(void);

  bool setTemperatureOffset(float offset, uint16_t delayMillis = 1);
  float getTemperatureOffset(void); // Will return zero if offset is invalid
  bool getTemperatureOffset(float *offset); // Returns true if offset is valid
  bool setSensorAltitude(uint16_t altitude, uint16_t delayMillis = 1);
  uint16_t getSensorAltitude(void); // Will return zero if offset is invalid
  bool getSensorAltitude(uint16_t *altitude); // Returns true if offset is valid
  bool setAmbientPressure(float pressure, uint16_t delayMillis = 1);

  float performForcedRecalibration(uint16_t concentration);
  bool performForcedRecalibration(uint16_t concentration, float *correction); // Returns true if FRC is successful
  bool setAutomaticSelfCalibrationEnabled(bool enabled = true, uint16_t delayMillis = 1);
  bool getAutomaticSelfCalibrationEnabled(void);
  bool getAutomaticSelfCalibrationEnabled(uint16_t *enabled);

  bool startLowPowerPeriodicMeasurement(void);
  bool getDataReadyStatus(void);

  bool persistSettings(uint16_t delayMillis = 800);
  bool getSerialNumber(char *serialNumber); // Returns true if serial number is read correctly
  bool performSelfTest(void);
  bool performFactoryReset(uint16_t delayMillis = 1200);
  bool reInit(uint16_t delayMillis = 20);

  bool measureSingleShot(void); // SCD41 only
  bool measureSingleShotRHTOnly(void); // SCD41 only

  bool sendCommand(uint16_t command, uint16_t arguments);
  bool sendCommand(uint16_t command);

  bool readRegister(uint16_t registerAddress, uint16_t *response, uint16_t delayMillis = 1);

  uint8_t computeCRC8(uint8_t data[], uint8_t len);

private:
  //Variables
#ifdef USE_TEENSY3_I2C_LIB
  i2c_t3 *_i2cPort = NULL; //The generic connection to user's chosen I2C hardware
#else
  TwoWire *_i2cPort = NULL;                                                                       //The generic connection to user's chosen I2C hardware
#endif

  //Sensor type
  scd4x_sensor_type_e _sensorType;

  //Global main datums
  float co2 = 0;
  float temperature = 0;
  float humidity = 0;

  //These track the staleness of the current data
  //This allows us to avoid calling readMeasurement() every time individual datums are requested
  bool co2HasBeenReported = true;
  bool humidityHasBeenReported = true;
  bool temperatureHasBeenReported = true;

  //Keep track of whether periodic measurements are in progress
  bool periodicMeasurementsAreRunning = false;

  //Convert serial number digit to ASCII
  char convertHexToASCII(uint8_t digit);

  //Debug
  Stream *_debugPort;          //The stream to send debug messages to if enabled. Usually Serial.
  boolean _printDebug = false; //Flag to print debugging variables
};
#endif
