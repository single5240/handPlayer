#include "letter_shell_task.h"

#include "cmsis_os.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
#include "drv_e28.h"
#include "event_mgr.h"
#include "shell.h"
#include "offline_task.h"
#include "SEGGER_RTT.h"
#include "usbd_cdc_if.h"
#define LOG_TAG "shell_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

#define CMD_BUFSIZE 128

Shell shell;
char shellBuffer[CMD_BUFSIZE];

fifo_s_t shell_fifo;
char shell_fifo_buf[CMD_BUFSIZE];
static offline_manage_obj_p offline_detector;

void shell_interupt(uint8_t *buff, uint16_t len)
{
    fifo_s_puts_noprotect(&shell_fifo, (char *)buff, len);
}

/**
 * @brief 用户shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    __log_output(data, len);
    return len;
}

/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
    return fifo_s_gets(&shell_fifo, data, len);
}

/// @brief 使用SEGGER_RTT作为shell读
/// @param data
/// @param len
/// @return
short userShellRead_by_Segger_RTT(char *data, unsigned short len)
{
    if (SEGGER_RTT_HasKey() == 1)
    {
        return SEGGER_RTT_Read(0, data, len);
    }
    else
    {
        return 0;
    }
}

void Shell_Task(void const *argument)
{
    char data;
    UNUSED(data);

    offline_detector = get_offline_manage();
    fifo_s_init(&shell_fifo, shell_fifo_buf, CMD_BUFSIZE);
    shell.write = userShellWrite;

#if (DEBUG_LOG_SHELL_METHOD)
    shell.read = userShellRead_by_Segger_RTT;
#else
    shell.read = userShellRead;
#endif

    osDelay(6000);
    shellInit(&shell, shellBuffer, CMD_BUFSIZE);
    // log_i(RTT_CTRL_RESET"Shell_Task_launch!");
    while (1)
    {
        if (shell.read && shell.read(&data, 1) == 1)
        {
            shellHandler(&shell, data);
        }
        osDelay(2);
    }
}

int func(int argc, char *agrv[])
{
    printf("%dparameter(s)\r\n", argc);
    for (char i = 1; i < argc; i++)
    {
        shellPrint(&shell,"hello, %s\r\n",agrv[i]);
        log_printf("hello, %s\r\n", agrv[i]);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), func, func, 测试命令);

int show_state(void)
{
    log_printf("radio state = %d, radio freq = %f\r\n", offline_detector_get_state(&offline_detector->radio), offline_detector_get_freq(&offline_detector->radio));
    log_printf("mbr1 state = %d, mbr1 freq = %f\r\n", offline_detector_get_state(&offline_detector->mbr1), offline_detector_get_freq(&offline_detector->mbr1));
    log_printf("mbr2 state = %d, mbr2 freq = %f\r\n", offline_detector_get_state(&offline_detector->mbr2), offline_detector_get_freq(&offline_detector->mbr2));
    log_printf("mbr3 state = %d, mbr3 freq = %f\r\n", offline_detector_get_state(&offline_detector->mbr3), offline_detector_get_freq(&offline_detector->mbr3));
    log_printf("io state = %d, io freq = %f\r\n", offline_detector_get_state(&offline_detector->io), offline_detector_get_freq(&offline_detector->io));
    return 1;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), info_state, show_state, 显示所有模块状态);
