#include "jaem_sleepmode.h"

extern ke_msg_id_t LED_BLINK_TIMER;

ke_msg_id_t connected_sleep;
bool CONNECTED_SLEEP_STATUS = false;
uint16_t CONNECTED_SLEEP_COUNT = 0;

//  "user_config.h"  ->  "Line 212  : user_default_hnd_conf" 

void extend_sleepmode_on(void)
{
	if (LED_BLINK_TIMER != 0xFFFF)
	{
			app_easy_timer_cancel(LED_BLINK_TIMER);
			LED_BLINK_TIMER = 0xFFFF;
	}

//	JAEM_function_off();
	set_tmr_enable(CLK_PER_REG_TMR_DISABLED);	// 타이머 클럭 종료
	app_easy_gap_advertise_stop();	// 게시모드 정지
  arch_set_extended_sleep();	// 확장 슬립모드 진입
  arch_ble_ext_wakeup_on(); 	// BLE External Wakeup 활성화
	app_button_enable(); // Wakeup 버튼 활성화
}


void app_wakeup_cb(void)
{
	// If state is not idle, ignore the message
	if (ke_state_get(TASK_APP) == APP_CONNECTABLE)
        EXECUTE_DEFAULT_OPERATION_VOID(default_operation_adv);
}

void app_button_press_cb(void)
{
	if(GetBits16(SYS_STAT_REG, PER_IS_DOWN))
			periph_init(); 
	
	if (arch_ble_ext_wakeup_get())
	{
			arch_set_sleep_mode(app_default_sleep_mode); //SetBits32(GP_CONTROL_REG, BLE_WAKEUP_REQ, 1);
			arch_ble_force_wakeup( );
			arch_ble_ext_wakeup_off();
			app_easy_wakeup(); // ke_msg_send_basic(APP_WAKEUP_MSG, TASK_APP, 0);
	}
	user_app_adv_start();
}

void app_button_enable(void)
{
	app_easy_wakeup_set(app_wakeup_cb);
	wkupct_register_callback(app_button_press_cb);
	if (GPIO_GetPinStatus( GPIO_BUTTON_PORT, GPIO_BUTTON_PIN ))
			wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), // select pin (GPIO_BUTTON_PORT, GPIO_BUTTON_PIN)
			WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, WKUPCT_PIN_POLARITY_LOW), // polarity low
			1, // 1 event
			0); // debouncing time = 0
}

void SLEEP_connected_on(void)
{
	if(CONNECTED_SLEEP_COUNT ==  CONNECTED_SLEEP_TIME_LIMIT) {
		JAEM_function_off();
		CONNECTED_SLEEP_STATUS = true;
	}
	else if(ke_state_get(TASK_APP) != APP_CONNECTABLE) {
		connected_sleep = app_easy_timer(CONNECTED_SLEEP_TIMER_DELAY,SLEEP_connected_on);
		CONNECTED_SLEEP_COUNT++;
	}
}

void SLEEP_connected_off(void)
{
	adc_on();
	led_on();
	CONNECTED_SLEEP_STATUS = false;
	SLEEP_connected_on();	
}
