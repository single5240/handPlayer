#include "init.h"

#include "main.h"
#include "drv_uart.h"
#include "letter_shell_task.h"

#include "event_mgr.h"
#include "easyflash.h"

#define LOG_TAG "init"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

publisher_t rc_data_Pub;
publisher_t radio_rx_Pub;

env_pack_t rc_env;

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

void *get_rc_env(void)
{
	return &rc_env;
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

uint32_t usart3_rx_callback(uint8_t *buff, uint16_t len)
{

	return 0;
}

uint32_t usart1_rx_callback(uint8_t *buff, uint16_t len)
{

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

	usart6_manage_init();
	usart6_rx_callback_register(usart6_rx_callback);

	usart2_manage_init();
	usart2_rx_callback_register(usart2_rx_callback);

	usart3_manage_init();
	usart3_rx_callback_register(usart3_rx_callback);

	usart1_manage_init();
	usart1_rx_callback_register(usart1_rx_callback);

	easyflash_init();

	env_auto_create();

	boot_times_cal();

	printf_all_env();
}

static void env_auto_create(void)
{
	size_t read_len;

	read_len = ef_get_env_blob("boot_times", &rc_env.sys.boot_times, sizeof(rc_env.sys.boot_times), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
		ef_set_env_blob("boot_times", 0, sizeof(rc_env.sys.boot_times));

	read_len = ef_get_env_blob("radio_addr", &rc_env.radio.addr, sizeof(rc_env.radio.addr), &read_len);
	uint16_t addr = (uint16_t)HAL_RNG_GetRandomNumber(&hrng);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
		ef_set_env_blob("radio_addr", &addr, sizeof(rc_env.radio.addr));

	read_len = ef_get_env_blob("radio_chan", &rc_env.radio.chan, sizeof(rc_env.radio.chan), &read_len);
	uint8_t chan = 5;
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
		ef_set_env_blob("radio_chan", &chan, sizeof(rc_env.radio.chan));

	read_len = ef_get_env_blob("radio_match_key", &rc_env.radio.match_key, sizeof(rc_env.radio.match_key), &read_len);
	uint16_t match_key = (uint16_t)HAL_RNG_GetRandomNumber(&hrng);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
		ef_set_env_blob("radio_match_key", &match_key, sizeof(rc_env.radio.match_key));

	read_len = ef_get_env_blob("turnback_record", &rc_env.turnback_record, sizeof(rc_env.turnback_record), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
	{
		rc_env.turnback_record.leftturn_360_leftwheel = 0;
		rc_env.turnback_record.leftturn_360_rightwheel = 0;
		rc_env.turnback_record.rightturn_360_leftwheel = 0;
		rc_env.turnback_record.rightturn_360_rightwheel = 0;
		rc_env.turnback_record.stright_1m = 0;
		ef_set_env_blob("turnback_record", &rc_env.turnback_record, sizeof(rc_env.turnback_record));
	}

	read_len = ef_get_env_blob("turnback_str_dis", &rc_env.turnback_str_dis, sizeof(rc_env.turnback_str_dis), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
	{
		rc_env.turnback_str_dis = 50;
		ef_set_env_blob("turnback_str_dis", &rc_env.turnback_str_dis, sizeof(rc_env.turnback_str_dis));
	}

	read_len = ef_get_env_blob("chassis_acceleration", &rc_env.chassis_acceleration, sizeof(rc_env.chassis_acceleration), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
	{
		rc_env.chassis_acceleration = 5;
		ef_set_env_blob("chassis_acceleration", &rc_env.chassis_acceleration, sizeof(rc_env.chassis_acceleration));
	}

	read_len = ef_get_env_blob("chassis_deceleration", &rc_env.chassis_deceleration, sizeof(rc_env.chassis_deceleration), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
	{
		rc_env.chassis_deceleration = 5;
		ef_set_env_blob("chassis_deceleration", &rc_env.chassis_deceleration, sizeof(rc_env.chassis_deceleration));
	}

	read_len = ef_get_env_blob("inverter_acceleration", &rc_env.inverter_acceleration, sizeof(rc_env.inverter_acceleration), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
	{
		rc_env.inverter_acceleration = 10;
		ef_set_env_blob("inverter_acceleration", &rc_env.inverter_acceleration, sizeof(rc_env.inverter_acceleration));
	}

	read_len = ef_get_env_blob("inverter_deceleration", &rc_env.inverter_deceleration, sizeof(rc_env.inverter_deceleration), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
	{
		rc_env.inverter_deceleration = 5;
		ef_set_env_blob("inverter_deceleration", &rc_env.inverter_deceleration, sizeof(rc_env.inverter_deceleration));
	}

	read_len = ef_get_env_blob("depended_turn", &rc_env.depended_turn, sizeof(rc_env.depended_turn), &read_len);
	if (read_len == 0) // 如果env不存在，直接用默认值覆写ENV。
	{
		rc_env.depended_turn = 0;
		ef_set_env_blob("depended_turn", &rc_env.depended_turn, sizeof(rc_env.depended_turn));
	}
}

void printf_all_env(void)
{
	size_t read_len;

	int32_t boot_times;
	ef_get_env_blob("boot_times", &boot_times, sizeof(boot_times), &read_len);
	log_i("boot_times = %d", boot_times);

	uint16_t radio_addr;
	ef_get_env_blob("radio_addr", &radio_addr, sizeof(radio_addr), &read_len);
	log_i("radio_addr = %d", radio_addr);

	uint8_t radio_chan;
	ef_get_env_blob("radio_chan", &radio_chan, sizeof(radio_chan), &read_len);
	log_i("radio_chan = %d", radio_chan);

	uint16_t match_key;
	ef_get_env_blob("radio_match_key", &match_key, sizeof(match_key), &read_len);
	log_i("match_key = %d", match_key);

	env_pack_t env;
	ef_get_env_blob("turnback_record", &env.turnback_record, sizeof(env.turnback_record), &read_len);

	log_i("env.turnback_record = \r\n \
			leftturn_360_leftwheel = %d \r\n \
			leftturn_360_rightwheel = %d \r\n \
			rightturn_360_leftwheel = %d \r\n \
			rightturn_360_rightwheel = %d \r\n \
			stright_1m = %d \r\n",
		  env.turnback_record.leftturn_360_leftwheel,
		  env.turnback_record.leftturn_360_rightwheel,
		  env.turnback_record.rightturn_360_leftwheel,
		  env.turnback_record.rightturn_360_rightwheel,
		  env.turnback_record.stright_1m);

	uint8_t turnback_str_dis;
	ef_get_env_blob("turnback_str_dis", &turnback_str_dis, sizeof(turnback_str_dis), &read_len);
	log_i("turnback_str_dis = %d", turnback_str_dis);

	uint8_t chassis_acceleration;
	ef_get_env_blob("chassis_acceleration", &chassis_acceleration, sizeof(chassis_acceleration), &read_len);
	log_i("chassis_acceleration = %d", chassis_acceleration);

	uint8_t chassis_deceleration;
	ef_get_env_blob("chassis_deceleration", &chassis_deceleration, sizeof(chassis_deceleration), &read_len);
	log_i("chassis_deceleration = %d", chassis_deceleration);

	uint8_t inverter_acceleration;
	ef_get_env_blob("inverter_acceleration", &inverter_acceleration, sizeof(inverter_acceleration), &read_len);
	log_i("inverter_acceleration = %d", inverter_acceleration);

	uint8_t inverter_deceleration;
	ef_get_env_blob("inverter_deceleration", &inverter_deceleration, sizeof(inverter_deceleration), &read_len);
	log_i("inverter_deceleration = %d", inverter_deceleration);

	uint8_t depended_turn;
	ef_get_env_blob("depended_turn", &depended_turn, sizeof(depended_turn), &read_len);
	log_i("depended_turn = %d", depended_turn);
}

static void boot_times_cal(void)
{
	int boot_times = 0;
	size_t read_len = 0;
	ef_get_env_blob("boot_times", &boot_times, sizeof(boot_times), &read_len);
	boot_times++;
	ef_set_env_blob("boot_times", &boot_times, sizeof(boot_times));
	//	log_i("boot_times = %d",boot_times);
}
