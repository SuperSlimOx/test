#ifndef _JAEM_BUTTON_H_
#define _JAEM_BUTTON_H_

#include "battery.h"
#include "custs1_task.h"
#include "user_custs_config.h"
#include "app_easy_timer.h"
#include "jaem_led.h"
#include "JAEM_ADC.h"

enum
{
	CLICK_INTERVAL = 50, // DOUBLE CLICK INTERVAL 50*10ms = 0.5s
	CHECK_INTERVAL = 10, //PRESS TIME COUNT INTERVAL 10*10 = 0.1s
};

void button_init(void);
static void button_callback(void);
static void button_clicked(void);
static void button_pressed(void);
static void length_value_reset(void);
static void send_msg_batterylevel(void);
static void send_msg_length(void);

#endif
