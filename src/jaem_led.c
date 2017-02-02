#include "jaem_led.h"

ke_msg_id_t LED_RAMP_TIMER = 0;
ke_msg_id_t LED_BLINK_TIMER = 0;
bool LED_STATE = OFF;

void led_init(void)
{
	timer2_stop();
	set_tmr_enable(CLK_PER_REG_TMR_ENABLED);
	set_tmr_div(CLK_PER_REG_TMR_DIV_8);
	timer2_init(HW_CAN_NOT_PAUSE_PWM_2_3_4, PWM_2_3_4_SW_PAUSE_ENABLED, PWM_FREQUENCY);
}

void led_on(void)
{
	timer2_enable(TRIPLE_PWM_ENABLED);
	timer2_set_pwm2_duty_cycle(PWM2_DUTY_STEP);
	timer2_set_sw_pause(PWM_2_3_4_SW_PAUSE_DISABLED);

	LED_STATE = ON;
}

void led_off(void)
{
	timer2_enable(TRIPLE_PWM_DISABLED);
	
	LED_STATE = OFF;
}

void led_blink(void)
{
	if(ke_state_get(TASK_APP) != APP_CONNECTED)
	{
		if(LED_STATE == OFF)
		{
			led_on();
		}			
		else
		{
			led_off();
		}
		LED_BLINK_TIMER = app_easy_timer(LED_BLINK_INTERVAL, led_blink);
	}
}

void led_ramp(void)
{
	static uint16_t ramp_level = 0;
	
	if(ramp_level == LED_RAMP_LEVEL_THRESHOLD)
	{
		led_off();
		ramp_level = 0;
		if(ke_state_get(TASK_APP) == APP_CONNECTED)
		{
			led_on();
		}
		else
		{
			LED_BLINK_TIMER = app_easy_timer(LED_BLINK_INTERVAL, led_blink);
		}
	}
	else
	{
		timer2_enable(TRIPLE_PWM_ENABLED);
		timer2_set_pwm2_duty_cycle(ramp_level);
		timer2_set_sw_pause(PWM_2_3_4_SW_PAUSE_DISABLED);
		ramp_level+=PWM2_DUTY_RATIO;
		
		LED_RAMP_TIMER = app_easy_timer(LED_RAMP_INTERVAL, led_ramp);
	}
}
