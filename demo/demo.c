/*
 * Copyright (c) 2022-2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>

#include "cmsis_vio.h"
#include "cmsis_os2.h"

#include "sds.h"
#include "sds_rec.h"

#include "sensor_drv.h"
#include "sensor_config.h"


#include "globalCGSettings.h"
#include "demo_scheduler.h"
#include "cg_status.h"


#ifdef ASYNCHRONOUS
// Configuration for asynchronous mode
#include "configs/asynchronous_with_temp.h"
#else
// Configuration for synchronous mode
#ifdef TEMPSENSOR
#include "configs/synchronous_with_temp.h"
#else 
#include "configs/synchronous_with_no_temp.h"
#endif
#endif

#ifndef SENSOR_POLLING_INTERVAL
#define SENSOR_POLLING_INTERVAL             50U  /* 50ms */
#endif

#ifndef SENSOR_BUF_SIZE
#define SENSOR_BUF_SIZE                     8192U
#endif

// Sensor identifiers
static sensorId_t sensorId_accelerometer              = NULL;
static sensorId_t sensorId_gyroscope                  = NULL;
static sensorId_t sensorId_temperatureSensor          = NULL;

// Sensor configuration
static sensorConfig_t *sensorConfig_accelerometer     = NULL;
static sensorConfig_t *sensorConfig_gyroscope         = NULL;
static sensorConfig_t *sensorConfig_temperatureSensor = NULL;

// SDS identifiers
static sdsId_t sdsId_accelerometer                    = NULL;
static sdsId_t sdsId_gyroscope                        = NULL;
static sdsId_t sdsId_temperatureSensor                = NULL;

#ifdef TIMED
static sdsId_t sdsId_accelerometer_timestamps = NULL;
#endif

// SDS CG Connection
#ifdef TIMED
static sds_timed_sensor_cg_connection_t sensorConn_accelerometer;
#else 
static sds_sensor_cg_connection_t sensorConn_accelerometer;
#endif
static sds_sensor_cg_connection_t sensorConn_gyroscope;
static sds_sensor_cg_connection_t sensorConn_temperatureSensor;

// SDS buffers
static uint8_t sdsBuf_accelerometer[SDS_BUF_SIZE_ACCELEROMETER];
static uint8_t sdsBuf_gyroscope[SDS_BUF_SIZE_ACCELEROMETER];
static uint8_t sdsBuf_temperatureSensor[SDS_BUF_SIZE_TEMPERATURE_SENSOR];

#ifdef TIMED
static uint8_t sdsBuf_accelerometer_timestamps[SDS_BUF_SIZE_ACCELEROMETER_TIMESTAMPS];
#endif 

// Recorder identifiers
static sdsRecId_t recId_accelerometer                 = NULL;
static sdsRecId_t recId_gyroscope                     = NULL;
static sdsRecId_t recId_temperatureSensor             = NULL;

// SDS CG Connection 
static sds_recorder_cg_connection_t recConn_accelerometer;
static sds_recorder_cg_connection_t recConn_gyroscope;
static sds_recorder_cg_connection_t recConn_temperatureSensor;

// Recorder buffers
static uint8_t recBuf_accelerometer[REC_BUF_SIZE_ACCELEROMETER];
static uint8_t recBuf_gyroscope[REC_BUF_SIZE_GYROSCOPE];
static uint8_t recBuf_temperatureSensor[REC_BUF_SIZE_TEMPERATURE_SENSOR];

// Temporary sensor buffer
static uint8_t sensorBuf[SENSOR_BUF_SIZE];

// Sensor close flag
static uint8_t close_flag = 0U;

// Event close sent flag
static uint8_t event_close_sent;

// Thread identifiers
static osThreadId_t thrId_demo           = NULL;
static osThreadId_t thrId_read_sensors   = NULL;
static osThreadId_t thrId_stream         = NULL;

#define EVENT_DATA_ACCELEROMETER        (1U << 0)
#define EVENT_DATA_GYROSCOPE            (1U << 1)
#define EVENT_DATA_TEMPERATURE_SENSOR   (1U << 2)
#define EVENT_BUTTON                    (1U << 3)
#define EVENT_DATA_MASK                 (EVENT_DATA_ACCELEROMETER     | \
                                         EVENT_DATA_GYROSCOPE         | \
                                         EVENT_DATA_TEMPERATURE_SENSOR)
#define EVENT_MASK                      (EVENT_DATA_MASK | EVENT_BUTTON)

#define EVENT_CLOSE                     (1U << 0)
// Event sent to compute graph thread
#define EVENT_STREAM_CANCEL             (1U << 4)

demoContext_t demoContext;

// CMSIS-Stream compute graph thread
static __NO_RETURN void cmsis_stream (void *argument) {
  int error=CG_SUCCESS;
  uint32_t nbIterations;
  demoContext_t *context=(demoContext_t*)argument;

  nbIterations = demo_scheduler(&error,context);
  printf("Nb iteration = %d\r\n",nbIterations);
  printf("Stream error = %d\r\n",error);

  osThreadExit();
}

// Read sensor thread
static __NO_RETURN void read_sensors (void *argument) {
  uint32_t num, buf_size;
  uint32_t timestamp;
  (void)   argument;

  timestamp = osKernelGetTickCount();
  for (;;) {
    if (close_flag == 0U) {
      if (sensorGetStatus(sensorId_accelerometer).active != 0U) {
        num = sizeof(sensorBuf) / sensorConfig_accelerometer->sample_size;
        num = sensorReadSamples(sensorId_accelerometer, num, sensorBuf, sizeof(sensorBuf));
        if (num != 0U) {
          #ifdef TIMED
          // Write samples with timing 
          buf_size = sensorConfig_accelerometer->sample_size;
          for(int i=0;i<num;i++)
          {
            num = sdsWrite(sdsId_accelerometer, sensorBuf + i*buf_size, buf_size);
            if (num != buf_size) {
               printf("%s: SDS write failed\r\n", sensorConfig_accelerometer->name);
            }
            else
            {
                timestamp = osKernelGetTickCount();
                num = sdsWrite(sdsId_accelerometer_timestamps, &timestamp, 4);
                if (num != 4) {
                   printf("%s: SDS timestamp write failed\r\n", sensorConfig_accelerometer->name);
                }
            }
          }
          #else
            buf_size = num * sensorConfig_accelerometer->sample_size;
            num = sdsWrite(sdsId_accelerometer, sensorBuf, buf_size);
            if (num != buf_size) {
               printf("%s: SDS write failed\r\n", sensorConfig_accelerometer->name);
            }
          #endif
        }
      }

      if (sensorGetStatus(sensorId_gyroscope).active != 0U) {
        num = sizeof(sensorBuf) / sensorConfig_gyroscope->sample_size;
        num = sensorReadSamples(sensorId_gyroscope, num, sensorBuf, sizeof(sensorBuf));
        if (num != 0U) {
          buf_size = num * sensorConfig_gyroscope->sample_size;
          num = sdsWrite(sdsId_gyroscope, sensorBuf, buf_size);
          if (num != buf_size) {
            printf("%s: SDS write failed\r\n", sensorConfig_gyroscope->name);
          }
        }
      }

      if (sensorGetStatus(sensorId_temperatureSensor).active != 0U) {
        num = sizeof(sensorBuf) / sensorConfig_temperatureSensor->sample_size;
        num = sensorReadSamples(sensorId_temperatureSensor, num, sensorBuf, sizeof(sensorBuf));
        if (num != 0U) {
          buf_size = num * sensorConfig_temperatureSensor->sample_size;
          num = sdsWrite(sdsId_temperatureSensor, sensorBuf, buf_size);
          if (num != buf_size) {
            printf("%s: SDS write failed\r\n", sensorConfig_temperatureSensor->name);
          }
        }
      }

    } else {
      if (event_close_sent == 0U) {
        event_close_sent = 1U;
        osThreadFlagsSet(thrId_demo, EVENT_CLOSE);
      }
    }

    timestamp += SENSOR_POLLING_INTERVAL;
    osDelayUntil(timestamp);
  }
}

// Button thread
static __NO_RETURN void button (void *argument) {
  uint32_t value, value_last = 0U;
  (void)   argument;

  for (;;) {
    // Monitor user button
    value = vioGetSignal(vioBUTTON0);
    if (value != value_last) {
      value_last = value;
      if (value == vioBUTTON0) {
        // Button pressed
        osThreadFlagsSet(thrId_demo, EVENT_BUTTON);
      }
    }
    osDelay(100U);
  }
}

// SDS event callback
static void sds_event_callback (sdsId_t id, uint32_t event, void *arg) {
  (void)arg;

  if ((event & SDS_EVENT_DATA_HIGH) != 0U) {
    if (id == sdsId_accelerometer) {
      osThreadFlagsSet(thrId_stream, EVENT_DATA_ACCELEROMETER);
    }
    if (id == sdsId_gyroscope) {
      osThreadFlagsSet(thrId_stream, EVENT_DATA_GYROSCOPE);
    }
    if (id == sdsId_temperatureSensor) {
      osThreadFlagsSet(thrId_stream, EVENT_DATA_TEMPERATURE_SENSOR);
    }
  }
}

// Recorder event callback
#ifdef RECORDER_USED
static void recorder_event_callback (sdsRecId_t id, uint32_t event) {
  if (event & SDS_REC_EVENT_IO_ERROR) {
    if (id == recId_accelerometer) {
      printf("%s: Recorder event - I/O error\r\n", sensorConfig_accelerometer->name);
    }
    if (id == recId_gyroscope) {
      printf("%s: Recorder event - I/O error\r\n", sensorConfig_gyroscope->name);
    }
    if (id == recId_temperatureSensor) {
      printf("%s: Recorder event - I/O error\r\n", sensorConfig_temperatureSensor->name);
    }
  }
}
#endif

// button_event
static void button_event (void) {
         uint32_t flags;
  static uint8_t  active = 0U;

  if (active == 0U) {
    active = 1U;

    thrId_stream = osThreadNew(cmsis_stream, (void*)&demoContext,NULL);

    // Accelerometer enable
    sdsClear(sdsId_accelerometer);
    sensorEnable(sensorId_accelerometer);
    printf("Accelerometer enabled\r\n");

    // Gyroscope enable
    sdsClear(sdsId_gyroscope);
    sensorEnable(sensorId_gyroscope);
    printf("Gyroscope enabled\r\n");

    // Temperature sensor enable
    sdsClear(sdsId_temperatureSensor);
    sensorEnable(sensorId_temperatureSensor);
    printf("Temperature sensor enabled\r\n");

  } else {
    event_close_sent = 0U;
    close_flag = 1U;
    flags = osThreadFlagsWait(EVENT_CLOSE, osFlagsWaitAny, osWaitForever);
    if ((flags & osFlagsError) == 0U) {
      active = 0U;

      osThreadFlagsSet(thrId_stream, EVENT_STREAM_CANCEL);
      thrId_stream = NULL;
      // Accelerometer disable
      sensorDisable(sensorId_accelerometer);
      printf("Accelerometer disabled\r\n");

      // Gyroscope disable
      sensorDisable(sensorId_gyroscope);
      printf("Gyroscope disabled\r\n");

      // Temperature sensor disable
      sensorDisable(sensorId_temperatureSensor);
      printf("Temperature sensor disabled\r\n");
    }

    close_flag = 0U;
  }
}

// Sensor Demo
void __NO_RETURN demo(void) {
  uint32_t  n, num, flags;

  thrId_demo = osThreadGetId();


  // Get sensor identifier
  sensorId_accelerometer     = sensorGetId("Accelerometer");
  sensorId_gyroscope         = sensorGetId("Gyroscope");
  #ifdef FAKE_SENSOR
  sensorId_temperatureSensor = sensorGetId("Fake");
  #else
  sensorId_temperatureSensor = sensorGetId("Temperature");
  #endif

  // Get sensor configuration
  sensorConfig_accelerometer     = sensorGetConfig(sensorId_accelerometer);
  sensorConfig_gyroscope         = sensorGetConfig(sensorId_gyroscope);
  sensorConfig_temperatureSensor = sensorGetConfig(sensorId_temperatureSensor);

  // Open SDS
  sdsId_accelerometer     = sdsOpen(sdsBuf_accelerometer,
                                    sizeof(sdsBuf_accelerometer),
                                    0U, SDS_THRESHOLD_ACCELEROMETER);
                                    
#if TIMED
  sdsId_accelerometer_timestamps     = sdsOpen(sdsBuf_accelerometer_timestamps,
                                    sizeof(sdsBuf_accelerometer_timestamps),
                                    0U, SDS_BUF_SIZE_ACCELEROMETER_TIMESTAMPS);
#endif

  sdsId_gyroscope         = sdsOpen(sdsBuf_gyroscope,
                                    sizeof(sdsBuf_gyroscope),
                                    0U, SDS_THRESHOLD_GYROSCOPE);

  sdsId_temperatureSensor = sdsOpen(sdsBuf_temperatureSensor,
                                    sizeof(sdsBuf_temperatureSensor),
                                    0U, SDS_THRESHOLD_TEMPERATURE_SENSOR);


  // Register SDS events
  sdsRegisterEvents(sdsId_accelerometer,     sds_event_callback, SDS_EVENT_DATA_HIGH, NULL);
  sdsRegisterEvents(sdsId_gyroscope,         sds_event_callback, SDS_EVENT_DATA_HIGH, NULL);
  sdsRegisterEvents(sdsId_temperatureSensor, sds_event_callback, SDS_EVENT_DATA_HIGH, NULL);

  // Init accelerometer sensor - CG connection datastructure
  sensorConn_accelerometer.event = EVENT_DATA_ACCELEROMETER; 
  sensorConn_accelerometer.cancel_event = EVENT_STREAM_CANCEL; 
  sensorConn_accelerometer.timeout = osWaitForever;
  sensorConn_accelerometer.sdsId = sdsId_accelerometer;
#if TIMED
  sensorConn_accelerometer.sdsTimestampsId = sdsId_accelerometer_timestamps;
#endif

  // Init accelerometer recorder - CG connection datastructure
  recConn_accelerometer.sensorName="Accelerometer";
  recConn_accelerometer.recorderBuffer=recBuf_accelerometer;
  recConn_accelerometer.recorderBufferSize=REC_BUF_SIZE_ACCELEROMETER;
  recConn_accelerometer.recorderThreshold=REC_IO_THRESHOLD_ACCELEROMETER;

  // Init gyroscope sensor - CG connection datastructure
  sensorConn_gyroscope.event = EVENT_DATA_GYROSCOPE; 
  sensorConn_gyroscope.cancel_event = EVENT_STREAM_CANCEL; 
  sensorConn_gyroscope.timeout = osWaitForever;
  sensorConn_gyroscope.sdsId = sdsId_gyroscope;

  // Init accelerometer recorder - CG connection datastructure
  recConn_gyroscope.sensorName="Gyroscope";
  recConn_gyroscope.recorderBuffer=recBuf_gyroscope;
  recConn_gyroscope.recorderBufferSize=REC_BUF_SIZE_GYROSCOPE;
  recConn_gyroscope.recorderThreshold=REC_IO_THRESHOLD_GYROSCOPE;

  // Init temperature sensor - CG connection datastructure
  sensorConn_temperatureSensor.event = EVENT_DATA_TEMPERATURE_SENSOR; 
  sensorConn_temperatureSensor.cancel_event = EVENT_STREAM_CANCEL; 
  sensorConn_temperatureSensor.timeout = osWaitForever;
  sensorConn_temperatureSensor.sdsId = sdsId_temperatureSensor;

  // Init temperature recorder - CG connection datastructure
  recConn_temperatureSensor.sensorName="Temperature";
  recConn_temperatureSensor.recorderBuffer=recBuf_temperatureSensor;
  recConn_temperatureSensor.recorderBufferSize=REC_BUF_SIZE_TEMPERATURE_SENSOR;
  recConn_temperatureSensor.recorderThreshold=REC_IO_THRESHOLD_TEMPERATURE_SENSOR;

  demoContext.sensorConn_accelerometer = &sensorConn_accelerometer;
  demoContext.recConn_accelerometer = &recConn_accelerometer;

  demoContext.sensorConn_gyroscope = &sensorConn_gyroscope;
  demoContext.recConn_gyroscope = &recConn_gyroscope;

  demoContext.sensorConn_temperatureSensor = &sensorConn_temperatureSensor;
  demoContext.recConn_temperatureSensor = &recConn_temperatureSensor;

#ifdef RECORDER_USED
  #ifdef RECORDER_USED
  // Initialize recorder
  int32_t err = sdsRecInit(recorder_event_callback);
  if (err != SDS_REC_OK)
  {
     printf("Error initializing recorder\r\n");
  }
  #endif

#endif

  // Create sensor thread
  thrId_read_sensors = osThreadNew(read_sensors, NULL, NULL);

  // Create button thread
  osThreadNew(button, NULL, NULL);

  for(;;) {
    flags = osThreadFlagsWait(EVENT_BUTTON, osFlagsWaitAny, osWaitForever);
    if ((flags & osFlagsError) == 0U) {

      // Button pressed event
      if (flags & EVENT_BUTTON) {
        printf("Button pressed\r\n");
        button_event();
      }

    }
  }
}
