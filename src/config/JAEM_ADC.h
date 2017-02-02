#ifndef JAEM_ADC_H
#define JAEM_ADC_H

#include "user_periph_setup.h"
#include "adc.h"
#include "systick.h"
#include "gpio.h"
#include "uart.h"

#define	WHITE_REF_FACTOR 1.1
#define	BLACK_REF_FACTOR 0.9

enum
{
	PATTERN_WHITE,
	PATTERN_BLACK,
};

enum
{
	UPPER_WHITE_LOWER_BLACK,
	UPPER_BLACK_LOWER_BLACK,
	UPPER_BLACK_LOWER_WHITE,
	UPPER_WHITE_LOWER_WHITE,
};

enum
{
	FORWARD = 1,
	FORWARD_EXCEPTION = -3,
	BACKWARD = -1,
	BACKWARD_EXCEPTION = 3,
};

enum
{
	MARGIN = 100,
	ADC_PERIOD_US = 140,
	RENEWAL_PERIOD = 5,
};

void jaem_adc_init(void);
void adc_on(void);
void adc_off(void);
void adc_wait(void);
static void adc_on_callback(void);
static void sampling_process(void);
static uint16_t sample_to_status(void);
static void counting_length(const uint16_t CURRENT_STATUS);
static void max_min_renewal(const uint16_t SAMPLE, uint16_t *MAX, uint16_t  *MIN);
static void ref_value_renewal(void);
static void max_min_init(void);
static void length_count_increase(void);
static void length_count_decrease(void);
static void length_count_stay(void) ;
static void set_color_ref(void);

#endif
