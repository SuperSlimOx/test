#include "JAEM_BUTTON.h"

ke_msg_id_t CLICK_TIMER = 0; // TIMER FOR DOUBLE CLICK
ke_msg_id_t PRESS_TIMER = 0; // TIMER FOR PRESS
bool BUTTON_CLICKED = false; // DOUBLE CLICK FLAG
uint16_t PRESS_COUNT = 0; // FOR PRESS TIME CHECK
uint8_t BATTERY_LEVEL = 0;

extern uint16_t JAEM_LENGTH; //from JAEM_ADC module
//extern bool CONNECTED_SLEEP_STATUS; //from JAEM_SLEEPMODE module
extern int CONNECTED_SLEEP_COUNT;

void button_init (void)
{
	GPIO_EnableIRQ(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, GPIO0_IRQn, true, true, 100);
	GPIO_RegisterCallback(GPIO0_IRQn, button_callback);
	BATTERY_LEVEL = battery_get_lvl(BATT_CR2032);
}

static void button_callback(void)
{
	if(ke_state_get(TASK_APP) == APP_CONNECTED) //APP CONNECTION CHECK
	{
		CONNECTED_SLEEP_COUNT = 0;
		
//		if(CONNECTED_SLEEP_STATUS == true) //SLEEP STATUS CHECK
//		{
//			SLEEP_connected_off();
//		}
//		else
//		{
			PRESS_TIMER = app_easy_timer(CHECK_INTERVAL, button_pressed); //button press timer on. check press time by 0.1s
			
			if(BUTTON_CLICKED == false)
			{
				BUTTON_CLICKED = true;
				CLICK_TIMER = app_easy_timer(CLICK_INTERVAL, button_clicked); //button double click check. click interval = 0.5s
			}
			else
			{
				send_msg_batterylevel();
			}
//		}
	}
}

static void button_clicked(void)
{
	if(ke_state_get(TASK_APP) == APP_CONNECTED)
	{
		if(BUTTON_CLICKED == true)
		{
			send_msg_length();
		}
	}
}

static void button_pressed(void)
{
	if(ke_state_get(TASK_APP) == APP_CONNECTED)
	{
		if(PRESS_COUNT != 30)
		{
			if(GPIO_GetPinStatus(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN) == false)
			{
				PRESS_COUNT++;
				PRESS_TIMER = app_easy_timer(CHECK_INTERVAL, button_pressed);
			}
			else
			{
				PRESS_COUNT = 0;
			}
		}
		else
		{
			length_value_reset();
		}
	}
}

static void length_value_reset(void)
{
		JAEM_LENGTH = 0;
		PRESS_COUNT = 0;
		send_msg_length();
		led_ramp();
}

static void send_msg_batterylevel(void)
{
	uint8_t TEMP_BATTERY;
	struct custs1_val_ntf_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																										TASK_CUSTS1,
																										TASK_APP,
																										custs1_val_ntf_req,
																										DEF_CUST1_ADC_VAL_2_CHAR_LEN);

	req->conhdl = app_env->conhdl;
	req->handle = CUST1_IDX_ADC_VAL_2_VAL;
	req->length = DEF_CUST1_ADC_VAL_2_CHAR_LEN;
	//sleep status check
//	if(CONNECTED_SLEEP_STATUS != true)
	{
		adc_wait();
	}
	//battery level check
	TEMP_BATTERY = battery_get_lvl(BATT_CR2032);
	BATTERY_LEVEL = (0.3 * BATTERY_LEVEL + 0.1 * TEMP_BATTERY) / (0.3+0.1);
	//msg upload
	memcpy(req->value, &BATTERY_LEVEL, req->length);
	ke_msg_send(req);
	
//	if(CONNECTED_SLEEP_STATUS != true)
	{
		adc_on();
	}
	
	BUTTON_CLICKED = false;
}

static void send_msg_length(void)
{
  struct custs1_val_ntf_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                    TASK_CUSTS1,
                                                    TASK_APP,
                                                    custs1_val_ntf_req,
                                                    DEF_CUST1_ADC_VAL_1_CHAR_LEN);

  req->conhdl = app_env->conhdl;
  req->handle = CUST1_IDX_ADC_VAL_1_VAL;
  req->length = DEF_CUST1_ADC_VAL_1_CHAR_LEN;
	
	memcpy(req->value, &JAEM_LENGTH, req->length);
	ke_msg_send(req);
	
	BUTTON_CLICKED = false;
}
