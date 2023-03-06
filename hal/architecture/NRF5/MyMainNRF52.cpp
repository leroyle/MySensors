/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define ARDUINO_MAIN
#include "Arduino.h"
#include "semphr.h"
#include "Adafruit_TinyUSB.h"

// DEBUG Level 1
#if CFG_DEBUG
// weak function to avoid compilation error with
// non-Bluefruit library sketch such as ADC read test
void Bluefruit_printInfo() __attribute__((weak));
void Bluefruit_printInfo() {}
#endif

// DEBUG Level 3
#if CFG_SYSVIEW
#include "SEGGER_SYSVIEW.h"
#endif

static TaskHandle_t  _loopHandle;
static TaskHandle_t  _customTaskHandle;

// bool _user_custom_task(void) __attribute__((weak));
// bool _user_custom_task_init(void) __attribute__((weak));
// volatile bool loopTaskCreated = false;

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

#define LOOP_STACK_SZ       (256*4)
#define CALLBACK_STACK_SZ   (256*3)

static void loop_task(void* arg)
{
  (void) arg;

#if CFG_DEBUG
  // If Serial is not begin(), call it to avoid hard fault
  if (!Serial) {
    //Serial.begin(115200);
    // Wait for Serial connection in debug mode
    while ( !Serial ) yield();
  }
  // delay(500);
  LOG_LV1("\nloop_task", "starting Loop task\n");

  dbgPrintVersion();
#endif

#ifndef UNIT_TEST
_begin(); // Startup MySensors library and run sketch begin()
#else
  setup();
#endif

#if CFG_DEBUG
 Bluefruit_printInfo();
#endif

  while (1)
  {


#ifndef UNIT_TEST
    // run MySensors stuff
    _process();  // Process incoming data
    if (loop) {
      loop(); // Call sketch loop
    }
    if (serialEventRun) {
      serialEventRun();
    }

#else
    loop();
#endif

    //yield(); // yield to run other task

    // Serial events
    if (serialEvent && serialEventRun) serialEventRun();
  }
}

// static void custom_task(void* arg)
// {
//   (void) arg;
//  // uint8_t lal = 1;
// #if CFG_DEBUG
//   // If Serial is not begin(), call it to avoid hard fault
//   if (!Serial) {
//     Serial.begin(115200);
//     // Wait for Serial connection in debug mode
//     while ( !Serial ) yield();
//     //while (!Serial.availableForWrite()) {}
//   }

//   Serial.println("\nstarting Lora task");
  
// #endif
//   if (_user_custom_task_init) {
//       _user_custom_task_init(); // Call sketch  task
//   }

//   while (1)
//   {
    
//     if (_user_custom_task) {
//       bool loraJoined = _user_custom_task(); // Call sketch  task
//       if (loraJoined && loopTaskCreated == false) {
//         // joined LoRaWan, start MySensors task
//         xTaskCreate( loop_task, "loop", LOOP_STACK_SZ, NULL, TASK_PRIO_LOW, &_loopHandle);
//         loopTaskCreated = true;
//       }
//     }
//     // if (serialEventRun) {
//     //   serialEventRun();
//     // }

//     yield(); // yield to run other task
//   }
// }
// \brief Main entry point of Arduino application

int main( void )
{
  init();
  initVariant();

#if CFG_SYSVIEW
  SEGGER_SYSVIEW_Conf();
#endif

#ifdef USE_TINYUSB
  TinyUSB_Device_Init(0);
#endif

  // Create a task for user custom task()
  //xTaskCreate( custom_task, "customTask", LOOP_STACK_SZ, NULL, TASK_PRIO_LOW, &_customTaskHandle);

  // Create a task for loop()
  // created elsewhere
  xTaskCreate( loop_task, "loop", LOOP_STACK_SZ, NULL, TASK_PRIO_LOW, &_loopHandle);

  // Initialize callback task
  ada_callback_init(CALLBACK_STACK_SZ);

  // Start FreeRTOS scheduler.
  vTaskStartScheduler();

  NVIC_SystemReset();

  return 0;
}

void suspendLoop(void)
{
  vTaskSuspend(_loopHandle);
}

void suspendLoraTask(void)
{
  vTaskSuspend(_customTaskHandle);
}

extern "C"
{

// nanolib printf() retarget
int _write (int fd, const void *buf, size_t count)
{
  (void) fd;

  if ( Serial )
  {
    return Serial.write( (const uint8_t *) buf, count);
  }
  return 0;
}

}