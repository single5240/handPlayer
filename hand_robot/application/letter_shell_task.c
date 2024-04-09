#include "letter_shell_task.h"

#include "cmsis_os.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
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

    fifo_s_init(&shell_fifo, shell_fifo_buf, CMD_BUFSIZE);
    shell.write = userShellWrite;

#if (DEBUG_LOG_SHELL_METHOD)
    shell.read = userShellRead_by_Segger_RTT;
#else
    shell.read = userShellRead;
#endif

    osDelay(6000);
    shellInit(&shell, shellBuffer, CMD_BUFSIZE);
    log_i(RTT_CTRL_RESET"Shell_Task_launch!");
    while (1)
    {
        if (shell.read && shell.read(&data, 1) == 1)
        {
            shellHandler(&shell, data);
        }
        osDelay(2);
    }
}

int hello(int argc, char *agrv[])
{
    printf("%dparameter(s)\r\n", argc);
    for (char i = 1; i < argc; i++)
    {
        // shellPrint(&shell,"hello, %s\r\n",agrv[i]);
        log_printf("%s:hello, my friend\r\n", agrv[i]);
    }
    return 8;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), hello, hello, 测试命令);
