#include "monitor_task.h"

#include "cmsis_os.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"

#define LOG_TAG "monitor_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

#if INCLUDE_uxTaskGetStackHighWaterMark
uint32_t Monitor_TaskStack;
#endif

void TaskUsageReport(void)
{
    char TSBuffer[512];
    char str[1024];
    char data1[] = "\r\n=======================================================\r\n";
    char data2[] = "任务名      任务状态  优先级  剩余栈  任务序号\r\n";
    char data4[] = "\r\n";

    vTaskList(TSBuffer);

    sprintf(str, "%s%s%s%s", data1, data2, TSBuffer, data4);

    __log_output(str, strlen(str));
}

void Monitor_Task(void const *argument)
{

    log_i("Monitor_Task_launch!");
    while (1)
    {
        TaskUsageReport();
        osDelay(1000);

#if INCLUDE_uxTaskGetStackHighWaterMark
        // Monitor_TaskStack = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}
