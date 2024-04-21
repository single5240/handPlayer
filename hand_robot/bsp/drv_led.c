#include "drv_led.h"

#include "drv_uart.h"
#include "init.h"
#include "fifo.h"


#define LOG_TAG "drv_led"
#define LOG_OUTPUT_LEVEL  LOG_INFO
#include "log.h"

static led_flash_t led_flash=
{
    .isLighted = 0,
    .flashInterval = 0
};

static void led_obj_init(led_s_obj_p obj, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, \
												 uint8_t (*set_led_f)(void *, uint8_t), \
												 uint8_t (*toggle_f)(void *), \
												 bool inv);
												 
uint8_t set_led(void *parm,uint8_t led_st);
static uint8_t led_toggle(void *this_p);



void led_obj_create(led_s_obj_p obj, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, bool inv)
{
		led_obj_init(obj,GPIOx,GPIO_Pin,set_led,led_toggle,inv);
}


uint8_t set_led(void *parm,uint8_t led_st)
{
		led_s_obj_p obj_p;
		uint8_t state;
	
		if(parm != NULL)
				obj_p = parm;
		else
				log_e("null pointer!");
		
		obj_p->state = led_st;
		
		if(obj_p->inverse == 1)
				state = !obj_p->state;
		else
				state = obj_p->state;
		
		if(state)
				HAL_GPIO_WritePin(obj_p->GPIOx,obj_p->GPIO_Pin,GPIO_PIN_SET);
		else
				HAL_GPIO_WritePin(obj_p->GPIOx,obj_p->GPIO_Pin,GPIO_PIN_RESET);
		
		return obj_p->state;
}	

static uint8_t led_toggle(void *this_p)
{
		led_s_obj_p obj_p = this_p;
	
		obj_p->state = !obj_p->state;
	
		obj_p->set_led_f(obj_p,obj_p->state);
	
		return obj_p->state;
}

static void led_obj_init(led_s_obj_p obj, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, \
												 uint8_t (*set_led_f)(void *, uint8_t), \
												 uint8_t (*toggle_f)(void *), \
												 bool inv)
{
		obj->GPIOx = GPIOx;
		obj->GPIO_Pin = GPIO_Pin;
		obj->set_led_f = set_led_f;
		obj->toggle_f = toggle_f;
		obj->inverse = inv;
}



void setMatchLightFlash(uint32_t interval)
{
    led_flash.flashInterval = interval;
    led_flash.isLighted = 1;
}

void setMatchLightOff(void)
{
    led_flash.isLighted = 0;
}

void setMatchLightOn(void)
{
    led_flash.isLighted = 2;
}


// static uint8_t matchIntervalFlag = 0;
//     static uint32_t matchAllIntervalTime = 0;
//     switch (ao_manage_obj.match_light_t.isLighted)
//     {
//         case 1: 
//         {
//             if(matchAllIntervalTime<=ao_manage_obj.light_manage_t.answerTime)
//             {
//                 matchAllIntervalTime=0;
//             }
//             else
//             {
//                 matchAllIntervalTime-=ao_manage_obj.light_manage_t.answerTime;
//                 return;
//             }

//             if(matchIntervalFlag != 1 )
//             {
//                 matchAllIntervalTime = ao_manage_obj.match_light_t.flashInterval;
//                 matchIntervalFlag = 1;
//             }
//             matchAllIntervalTime = ao_manage_obj.match_light_t.flashInterval;
//             log_i("matchAllIntervalTime?\r\n");
//             static uint8_t matchFlashturn = 0;
//             if(matchFlashturn==0)
//             {
//                 HAL_GPIO_WritePin(AN1_LED_GPIO_Port, AN1_LED_Pin, GPIO_PIN_RESET);
//                 matchFlashturn = 1;
//             }
//             else if(matchFlashturn == 1)
//             {
//                 HAL_GPIO_WritePin(AN1_LED_GPIO_Port, AN1_LED_Pin, GPIO_PIN_SET);
//                 matchFlashturn = 0;
//             }
//             break;
//         }      
//         case 0:
//         {
//             HAL_GPIO_WritePin(AN1_LED_GPIO_Port, AN1_LED_Pin, GPIO_PIN_RESET);
//             matchIntervalFlag = 0;          
//             break;
//         }   
//         case 2:
//         {
//             if(matchIntervalFlag ==2) return;
//             HAL_GPIO_WritePin(AN1_LED_GPIO_Port, AN1_LED_Pin, GPIO_PIN_SET);
//             matchIntervalFlag = 2;
//             break;
//         }
//         default:
//             break;
//     }

