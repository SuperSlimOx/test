#ifndef _JAEM_SLEEPMODE_H_
#define _JAEM_SLEEPMODE_H_

#include "user_periph_setup.h"
#include "user_peripheral.h"
#include "wkupct_quadec.h"       
#include "user_callback_config.h"
#include "jaem_led.h"
#include "JAEM_ADC.h"
#include "gpio.h"
#include "pwm.h"

#define SLEEP_MODE_MODULE
#define CONNECTED_SLEEP_TIME_LIMIT 3 // sec

void extend_sleepmode_on(void);
void app_button_enable(void);
void app_wakeup_cb(void);
void app_button_press_cb(void);
void SLEEP_connected_on(void);
void SLEEP_connected_off(void);

#endif
