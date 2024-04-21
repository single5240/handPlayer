#include "init.h"

#include "main.h"
#include "drv_uart.h"
#include "letter_shell_task.h"
#include "drv_comm.h"
#include "event_mgr.h"
#include "easyflash.h"

#define LOG_TAG "init"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

publisher_t rc_data_Pub;
publisher_t radio_rx_Pub;

extern RNG_HandleTypeDef hrng;

static void boot_times_cal(void);
static void env_auto_create(void);

uint32_t get_time_us(void)
{
	return TIM7->CNT;
}

uint32_t get_time_ms(void)
{
	return HAL_GetTick();
}

float get_time_ms_us(void)
{
	return get_time_ms() + get_time_us() / 1000.0f;
}

/**
 * @brief  usart1 interupt, debug shell
 * @param
 * @retval void
 */
uint32_t uart5_rx_callback(uint8_t *buff, uint16_t len)
{
	shell_interupt(buff, len);

	//		__log_output(buff, len);
	//		usart6_transmit(buff,len);
	return 0;
}

/**
 * @brief  usart3 interupt
 * @param
 * @retval void
 */
uint32_t usart6_rx_callback(uint8_t *buff, uint16_t len)
{

	return 0;
}

uint32_t usart2_rx_callback(uint8_t *buff, uint16_t len)
{

	return 0;
}
uint8_t usartFlag  = 0;
uint32_t usart3_rx_callback(uint8_t *buff, uint16_t len)
{
	return 0;
}

uint32_t usart1_rx_callback(uint8_t *buff, uint16_t len)
{
	// shell_interupt(buff, len);
	rxDataHandler(buff, len);
	return 0;
}

/**
 * @brief  board init
 * @param
 * @retval void
 */
void board_config(void)
{
	/* system log */
	uart5_manage_init();
	uart5_rx_callback_register(uart5_rx_callback);

	// usart6_manage_init();
	// usart6_rx_callback_register(usart6_rx_callback);

	usart2_manage_init();
	usart2_rx_callback_register(usart2_rx_callback);

	usart3_manage_init();
	usart3_rx_callback_register(usart3_rx_callback);

	usart1_manage_init();
	usart1_rx_callback_register(usart1_rx_callback);

	easyflash_init();

	env_auto_create();

	boot_times_cal();
}

static void env_auto_create(void)
{
	
}

static void boot_times_cal(void)
{
	// int boot_times = 0;
	// size_t read_len = 0;
	// ef_get_env_blob("boot_times", &boot_times, sizeof(boot_times), &read_len);
	// boot_times++;
	// ef_set_env_blob("boot_times", &boot_times, sizeof(boot_times));
	// //	log_i("boot_times = %d",boot_times);
}
