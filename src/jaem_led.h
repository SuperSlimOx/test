#ifndef _JAEM_LED_H_
#define _JAEM_LED_H_

#include "pwm.h"

enum
{
	PWM_FREQUENCY = 2000, // 1ms
	PWM2_DUTY_STEP = 100,
	PWM2_DUTY_RATIO = 30,
	LED_BLINK_INTERVAL = 100, // 100 * 10ms = 1s
	LED_RAMP_INTERVAL = 10, // 10 * 10ms = 0.1s
	LED_RAMP_LEVEL_THRESHOLD = 300, // 10 level max
};

enum
{
	OFF = false,
	ON = true,
};

void led_init(void);
void led_on(void);
void led_off(void);
void led_blink(void);
void led_ramp(void);

#endif
