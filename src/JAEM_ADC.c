#include "JAEM_ADC.h"

uint16_t JAEM_LENGTH = 0;
uint16_t SAMPLE1 = 0;
uint16_t SAMPLE2 = 0;
uint16_t SAMPLE1_REF_VALUE = 0;
uint16_t SAMPLE1_REF_VALUE_WHITE = 0;
uint16_t SAMPLE1_REF_VALUE_BLACK = 0;
uint16_t SAMPLE2_REF_VALUE = 0;
uint16_t SAMPLE2_REF_VALUE_WHITE = 0;
uint16_t SAMPLE2_REF_VALUE_BLACK = 0;
uint16_t SAMPLE1_MAX = 0;
uint16_t SAMPLE1_MIN = 2000;
uint16_t SAMPLE2_MAX = 0;
uint16_t SAMPLE2_MIN = 2000;
uint16_t REF_VALUE_RENEW_CNT = 0;
uint16_t TEMP_STATUS = UPPER_WHITE_LOWER_WHITE;

extern int CONNECTED_SLEEP_COUNT;
//------------------------------for test 

void printf_byte(char ch)
{
    // print a Byte in hex format
    char b;
    b = ((0xF0 & ch) >> 4);
    b += (b < 10) ? 48 : 55;

    uart2_write((uint8_t *)&b,1, NULL);
    uart2_finish_transfers();
    b = (0xF & ch);
    b += (b < 10) ? 48 : 55;
    uart2_write((uint8_t *)&b,1, NULL);
    uart2_finish_transfers();
}

void printf_string(char * str)
{
    uart2_write((uint8_t *)str, strlen(str), NULL); // send next string character
    uart2_finish_transfers();
}

void print_hword(uint16_t aHalfWord)
{
    printf_byte((aHalfWord >> 8) & 0xFF);
    printf_byte((aHalfWord) & 0xFF);
}

void reverse(char s[])
 {
    int i, j;
    char c;
 
    for (i = 0, j = strlen( s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
 }

 void itoa2(uint16_t n,uint16_t n2, char s[])
{
    int i;
    i = 0;
		s[i++] = '\n';
    do {       
        s[i++] = n % 10 + '0';   
    } while ((n /= 10) > 0);    
		s[i++] = ',';
		do  {    
        s[i++] = n2 % 10 + '0'; 
    } while ((n2 /= 10) > 0);
		s[i++] = '\r';
    s[i] = '\0';
		reverse(s);
}
//-----------------------------------------------------------------------
void jaem_adc_init(void)
{
	adc_calibrate();
	
	SAMPLE1 = 0; SAMPLE2 = 0; JAEM_LENGTH = 0;
	TEMP_STATUS = UPPER_WHITE_LOWER_WHITE;
	max_min_init();
	
	if(GPIO_GetPinStatus(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN))
	{
		GPIO_SetInactive(GPIO_EMITTER_PORT, GPIO_EMITTER_PIN);
	}
	
	for(uint8_t i = 0; i < 5; i++)
	{
		SAMPLE1 += adc_sampling(ADC_CHANNEL_P00);
		SAMPLE2 += adc_sampling(ADC_CHANNEL_P01);
		adc_usDelay(10);
	}
	
	GPIO_SetActive(GPIO_EMITTER_PORT, GPIO_EMITTER_PIN);
	
	SAMPLE1_REF_VALUE = (SAMPLE1/5) - MARGIN;
	SAMPLE2_REF_VALUE = (SAMPLE2/5) - MARGIN;
	set_color_ref();
	
	adc_on();
}

void adc_on(void)
{
	systick_register_callback(adc_on_callback);
	systick_start(ADC_PERIOD_US, 1);
}

void adc_off(void)
{
	systick_stop();
	GPIO_SetActive(GPIO_EMITTER_PORT, GPIO_EMITTER_PIN);
}

void adc_wait(void)
{
	systick_stop();
}

static void adc_on_callback(void)
{
	volatile static uint16_t EMITTER_DUTY = 1;
	
	if(EMITTER_DUTY == 1)
	{
		GPIO_SetInactive(GPIO_EMITTER_PORT, GPIO_EMITTER_PIN);
	}
	else if(EMITTER_DUTY == 2)
	{
		sampling_process();
		GPIO_SetActive(GPIO_EMITTER_PORT, GPIO_EMITTER_PIN);
	}
	else
	{
		EMITTER_DUTY = 0;
	}

	EMITTER_DUTY++;
}

static void sampling_process()
{
	for(uint8_t i = 0; i < 5; i++)
	{
		SAMPLE1 += adc_sampling(ADC_CHANNEL_P00);
		SAMPLE2 += adc_sampling(ADC_CHANNEL_P01);
		adc_usDelay(1);
	}
	
	SAMPLE1 = SAMPLE1/5;
	SAMPLE2 = SAMPLE2/5;
//test-------------------------	
//	static int print_count = 0;
//	char buffer[20];
//	if(print_count == 5)
//	{
//		itoa2(SAMPLE2, SAMPLE1,buffer);
//		printf_string(buffer);
//		print_count = 0;
//	}
//	print_count++;
	
	counting_length(sample_to_status());
}

static uint16_t sample_to_status(void)
{
	static uint16_t upper_IRsensor, lower_IRsensor;
	static uint16_t sensorStatus;
	
	// finding each sensor's color for next step
	if(SAMPLE1 > SAMPLE1_REF_VALUE_WHITE)						upper_IRsensor = PATTERN_WHITE;
	else if(SAMPLE1 < SAMPLE1_REF_VALUE_BLACK)		upper_IRsensor = PATTERN_BLACK;
	
	if(SAMPLE2 > SAMPLE2_REF_VALUE_WHITE)						lower_IRsensor = PATTERN_WHITE;
	else if(SAMPLE2 < SAMPLE2_REF_VALUE_BLACK)		lower_IRsensor = PATTERN_BLACK;
	
	 // decide on current status
	 if ((upper_IRsensor == PATTERN_BLACK) && (lower_IRsensor == PATTERN_WHITE))
           sensorStatus = UPPER_BLACK_LOWER_WHITE;
   else if ((upper_IRsensor == PATTERN_BLACK) && (lower_IRsensor == PATTERN_BLACK))
           sensorStatus = UPPER_BLACK_LOWER_BLACK;
   else if ((upper_IRsensor == PATTERN_WHITE) && (lower_IRsensor == PATTERN_BLACK))                   
           sensorStatus = UPPER_WHITE_LOWER_BLACK; 
   else
           sensorStatus = UPPER_WHITE_LOWER_WHITE;
	 
	 return sensorStatus;
}

static void counting_length(const uint16_t CURRENT_STATUS)
{
	int16_t DIRECTION = CURRENT_STATUS - TEMP_STATUS;
	
	if(DIRECTION == FORWARD || DIRECTION == FORWARD_EXCEPTION)
		length_count_increase();
	else if(DIRECTION == BACKWARD || DIRECTION == BACKWARD_EXCEPTION)
		length_count_decrease();
	else
		length_count_stay();
	
	TEMP_STATUS = CURRENT_STATUS;
	
	if(REF_VALUE_RENEW_CNT > RENEWAL_PERIOD)
		ref_value_renewal();
}

static void ref_value_renewal(void)
{	
	SAMPLE1_REF_VALUE = ( SAMPLE1_MAX + SAMPLE1_MIN ) / 2 ;
	SAMPLE2_REF_VALUE = ( SAMPLE2_MAX + SAMPLE2_MIN ) / 2 ;
	set_color_ref();
	
	max_min_init();
}

static void max_min_init(void)
{
	REF_VALUE_RENEW_CNT = 0;

	SAMPLE1_MAX = 0;
	SAMPLE1_MIN = 2000;
	SAMPLE2_MAX = 0;
	SAMPLE2_MIN = 2000;
}

static void length_count_increase(void)
{    
	JAEM_LENGTH ++;
	
	REF_VALUE_RENEW_CNT ++;
	CONNECTED_SLEEP_COUNT = 0;

//test-------------------------
	print_hword(JAEM_LENGTH);
	printf_string("\n\r");
}

static void length_count_decrease(void)
{    
  if ( JAEM_LENGTH > 0 )
       JAEM_LENGTH --;
						
	REF_VALUE_RENEW_CNT ++;
	CONNECTED_SLEEP_COUNT = 0;

//test-------------------------		
	print_hword(JAEM_LENGTH);
	printf_string("\n\r");
}

static void length_count_stay(void) 
{
	max_min_renewal(SAMPLE1, &SAMPLE1_MAX, &SAMPLE1_MIN);
	max_min_renewal(SAMPLE2, &SAMPLE2_MAX, &SAMPLE2_MIN);
}

static void max_min_renewal(const uint16_t SAMPLE, uint16_t *MAX, uint16_t  *MIN)
{
	if(SAMPLE > *MAX)
		*MAX = SAMPLE;
	else if(SAMPLE < *MIN)
		*MIN = SAMPLE;
}

static void set_color_ref(void)
{
	SAMPLE1_REF_VALUE_WHITE = SAMPLE1_REF_VALUE*WHITE_REF_FACTOR;
	SAMPLE1_REF_VALUE_BLACK = SAMPLE1_REF_VALUE*BLACK_REF_FACTOR;
	SAMPLE2_REF_VALUE_WHITE = SAMPLE2_REF_VALUE*WHITE_REF_FACTOR;
	SAMPLE2_REF_VALUE_BLACK = SAMPLE2_REF_VALUE*BLACK_REF_FACTOR;
}
