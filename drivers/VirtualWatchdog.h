/*
 * Copyright (c) 2019 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBED_VIRTUAL_WATCHDOG_H
#define MBED_VIRTUAL_WATCHDOG_H

#ifdef DEVICE_WATCHDOG

#include <cstdio>
#include "platform/mbed_error.h"
#include "platform/mbed_critical.h"
#include "platform/mbed_power_mgmt.h"
#include "platform/mbed_assert.h"
#include "platform/SingletonPtr.h"

#if DEVICE_LPTICKER
#include "drivers/LowPowerTicker.h"
#else
#include "drivers/Ticker.h"
#endif
namespace mbed {

/** \addtogroup drivers */

/** VirtualWatchdog should be used for applications where you use multiple services requiring watchdog functionality.
 *   Use `Watchdog` driver for simple uses cases like bootloader.
 *
 *  A system timer that will reset the system in the case of system failures or
 *  malfunctions.
 *
 * Example:
 * @code
 *
 * Example:
 * @code
 *
 * VirtualWatchdog virtual_watchdog(300,"Software Watchdog");
 * virtual_watchdog.start();
 *
 * while (true) {
 *    virtual_watchdog.kick();
 *
 *    // Application code
 * }
 * @endcode
 * @ingroup platform
 */
class VirtualWatchdog {
public:

    /** Constructor configured with timeout and name for this software watchdog instance.
     *
     * We start hardware watchdog on  the first run. This set-up uses default watchdog timeout set via config value
     * MBED_CONF_TARGET_WATCHDOG_TIMEOUT.
     *
     */
    VirtualWatchdog(uint32_t timeout = 1000, const char *const str = NULL);
    ~VirtualWatchdog();
public:

    /** Start an independent watchdog timer with specified parameters.
     *
     * Assert for multiple calls of start.
     */
    void start();

    /** This stops the watchdog timer.
     *
     * Calling this function disables any running
     * watchdog timers if the platform supports them.
     *
     * Assert without calling start.
     */
    void stop();

    /** This function refreshes the watchdog timer.
     *
     * Call this function periodically before the watchdog times out.
     * Otherwise, the system resets.
     *
     * If the watchdog timer is not running, this function does nothing.
     */
    void kick();

protected:

    /** Use add_to_list to store the registered user in the list.
      * This API is only used to call from start.
      */
    void add_to_list();

    /** Use remove_from_list to remove the entry from the list.
      * This API is only used to call from stop.
      *
      */
    void remove_from_list();

private:
    /** Periodic ticker handler to go through all the registered user/threads of watchdog.
     *
     * Otherwise, the system resets.
     */
    void process();

    uint32_t _timeout; //_timeout initialized via constructor while creating instance of this class
    const char *_name; //To store the details of user
    uint32_t _current_count; //this parameter is used to reset everytime threads/user calls kick
    bool _is_initialized; //To control start and stop functionality
    static bool _is_hw_watchdog_running; // track we are the first owner of watchdog
    static VirtualWatchdog *_first; //List to store the user/threads who called start
    VirtualWatchdog *_next;

#if DEVICE_LPTICKER
    /** Create singleton instance of LowPowerTicker for watchdog periodic call back of kick.
     */
    static SingletonPtr<LowPowerTicker> _ticker;
#else
    /** Create singleton instance of Ticker for watchdog periodic call back of kick.
     */
    static SingletonPtr<Ticker> _ticker;
#endif
    static us_timestamp_t _ticker_timeout;
};

} // namespace mbed

#endif // DEVICE_WATCHDOG
#endif // MBED_VIRTUAL_WATCHDOG_H