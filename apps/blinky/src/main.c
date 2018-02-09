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

/* For LED toggling */
int g_led_pin;

/**
 * main
 *
 * The main task for the project. This function initializes packages,
 * and then blinks the BSP LED in a loop.
 *
 * @return int NOTE: this function should never return!
 */
int
main(int argc, char **argv)
{
    int rc;

    sysinit();

    g_led_pin = LED_BLINK_PIN;
    hal_gpio_init_out(g_led_pin, 1);

    static const struct nrf52_hal_i2c_cfg hal_i2c_cfg = {
        .scl_pin = 27,
        .sda_pin = 26,
        .i2c_frequency = 100    /* 100 kHz */
    };

    rc = hal_i2c_init(0, (void *)&hal_i2c_cfg);
    assert(rc == 0);

    while (1) {

        // Attempt dummy i2c write
        uint8_t wbuff[1] = {0};
        struct hal_i2c_master_data pdata = {
            .address = 0x44,
            .len = 1,
            .buffer = wbuff
        };
        hal_i2c_master_write(0, &pdata, 100, 1);

        // Write will fail and SCL will get stuck low

        /* Wait one second */
        os_time_delay(OS_TICKS_PER_SEC);


        // Attempt to recover SCL (doesn't work for me)
        rc = hal_i2c_init(0, (void *)&hal_i2c_cfg);
        assert(rc == 0);

        /* Wait one second */
        os_time_delay(OS_TICKS_PER_SEC * 2);

        /* Toggle the LED */
        hal_gpio_toggle(g_led_pin);
    }
    assert(0);

    return rc;
}

