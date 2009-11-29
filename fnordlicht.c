/* vim:ts=4 sts=4 et tw=80
 *
 *         fnordlicht firmware
 *
 *    for additional information please
 *    see http://lochraster.org/fnordlicht
 *
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *     Lars Noschinski <lars@public.noschinski.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* includes */
#include "config.h"

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "common.h"
#include "pwm.h"
#include "uart.h"
#include "remote.h"
#include "timer.h"
#include "script.h"
#include "storage.h"

static void startup(void)
{
    /* if configuration is valid */
    if (storage_valid()) {

        /* read default mode from storage */
        switch (startup_config.startup_mode) {

            case STARTUP_PROGRAM:
                /* start program */
                script_start(0, startup_config.params.program, (union program_params_t *)startup_config.params.program_parameters);
                break;

            case STARTUP_STATIC:
                /* fade to target color */
                pwm_fade_rgb(&startup_config.params.color, startup_config.params.step, startup_config.params.delay);
                break;
        }
    } else {
        /* start default program */
        script_start_default();

#if !CONFIG_SCRIPT
        /* or set some default color */
        global_pwm.target.red = 50;
#endif
    }
}

/** main function
 */
int main(void)
{
    pwm_init();
    timer_init();
    uart_init();
    storage_init();
    remote_init();
    script_init();

    /* do high-level startup configuration */
    startup();

    /* enable interrupts globally */
    sei();

    while (1)
    {
        /* update pwm */
        pwm_poll();

        /* check for remote commands */
        remote_poll();

        /* update pwm */
        pwm_poll();

        /* call scripting */
        script_poll();

        /* update pwm */
        pwm_poll();

        /* update fading */
        pwm_poll_fading();
    }
}
