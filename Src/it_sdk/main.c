/* ==========================================================
 * main.c - Main Library Starting point
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 2 sept. 2018
 *     Author: Paul Pinault aka Disk91
 * ----------------------------------------------------------
 * Copyright (C) 2018 Disk91
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU LESSER General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------
 * 
 * This file is the main entry point for the Disk91 SDK
 * The project using the SDK must in its main function have the
 * Following content:
 * main() {
 *    // Needed project specific configuration
 *    ...
 *    // Call the IT_SDK Setup
 *    itsdk_setup();
 *
 *    // Loop forever
 *    while ( 1 ) {
 *      itsdk_loop();
 *    }
 * }
 *
 * The itsdk_setup() will make the sdk init then call the user function
 * project_setup(). This function have to be defined in your project for
 * executing the project specific setup.
 *
 * The itsdk_loop() will manage the low power execution and regularly call the
 * project_loop() function. It also managed the sdk scheduler. This function is
 * basically executed on each MCU wake-up. it depends on the hardware implementation.
 *
 * In case of MCU waking up from sleep with a MCU Reset (ex : ESP8266) a itsdk_restart()
 * function will be called instead of itsdk_setup()
 *
 * See each of the function for details.
 *
 * ----------------------------------------------------------
 * @TODO - Backlog features
 *
 * ==========================================================
 */
#include <it_sdk/itsdk.h>
#include <it_sdk/lowpower/lowpower.h>
#include <it_sdk/sched/scheduler.h>
#include <it_sdk/time/time.h>
#include <it_sdk/time/timer.h>
#include <it_sdk/logger/logger.h>

/**
 * The setup function is called on every MCU Reset but not on wakeup from sleep
 * This function init the SDK library and underlaying hardware.
 * Then it calls the project specific setup function
 */
void itsdk_setup() {

	itsdk_time_init();
	#if ITSDK_WDG_MS > 0
	  wdg_setupWithMaxMs(ITSDK_WDG_MS);
	#endif
	project_setup();
}

/**
 * The restart function is called when the MCU wakeup from lowpower mode with a full
 * reset like for ESP8266 to setup the itsdk without passing through itsdk_setup function.
 */
void itsdk_restart() {

	itsdk_loop();
}


/**
 * The loop function is called everytime the MCU is waking up or in a cycling loop
 * if the MCU have no lowpower mode. The function update the time component, call
 * all the recurrent SDK operations to be maintained.
 * When a scheduler has been activated it calls the scheduler task when needed.
 * Then is calls the project specific loop function.
 */
void itsdk_loop() {
	#if ITSDK_WDG_MS > 0
	   wdg_refresh();
	#endif
	#if ITSDK_SHEDULER_TASKS > 0
	   itdt_sched_execute();
	#endif
	#if ITSDK_TIMER_SLOTS > 0
	   itsdk_stimer_run();
	#endif
	project_loop();
	lowPower_switch();
}

/**
 * Error Handler for ItSdk internal
 * (The STM32 prototype is not stable over FW version)
 */
void itsdk_error_handler(char * file, int line) {
  #if ITSDK_LOGGER_CONF > 0
   log_debug("Error : %s (%d)\r\n",file,line);
  #endif
   while(1);
}



