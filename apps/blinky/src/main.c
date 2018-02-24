/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <string.h>

#include "sysinit/sysinit.h"
#include "os/os.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"
#include "mcu/nrf52_hal.h"
#include <shell/shell.h>
#include <console/console.h>
#include <nrf.h>

#define BLINK_TASK_PRI         (10)
#define BLINK_STACK_SIZE       (256)
struct os_task blink_task;
os_stack_t blink_task_stack[BLINK_STACK_SIZE];

static int i2c_fix_cli_cmd(int argc, char **argv);

static struct shell_cmd i2c_fix_cmd_struct = {
    .sc_cmd = "i2c_fix",
    .sc_cmd_func = i2c_fix_cli_cmd
};

#define NRF52_SCL_PIN_CONF                                              \
    ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |          \
      (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos) |          \
      (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos) |           \
      (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |          \
      (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos))


static int
i2c_fix_cli_cmd(int argc, char **argv) {
    console_printf("Trying to recover\n");

    NRF_TWIM0->TASKS_STOP = 1;
    return 0;
}

void blink_task_func(void *arg) {

    console_printf("I2C Test!\n");

    hal_gpio_init_out(LED_BLINK_PIN, 1);

    while (1) {
        os_time_delay(OS_TICKS_PER_SEC);
        hal_gpio_toggle(LED_BLINK_PIN);
    }
}

//
// Test runs on the nrf52dk (PCA10040)
// To test i2c bug, load on the nrf52 (newt run nrf52dk)
// Measure pin p0.27 (SCL) with a multimeter, it should be pulled high
// then connect over serial terminal and run the command 'i2c_scan 0'
// Measure p0.27 again and it will be low, which it shouldn't
// Note that there are no devices connected that could be pulling SCL low
// Now run the command 'i2c_fix' and measure p0.27 again
// It should be high

int
main(int argc, char **argv)
{
    int rc;

    sysinit();

    os_task_init(
        &blink_task,
        "blink_task",
        blink_task_func,
        NULL,
        BLINK_TASK_PRI,
        OS_WAIT_FOREVER,
        blink_task_stack,
        BLINK_STACK_SIZE);

    shell_cmd_register(&i2c_fix_cmd_struct);

    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
    assert(0);
    return rc;
}
