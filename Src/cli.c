/**********************************************************************************************************************
 * @file    cli.c
 * @author  Simon Benoit
 * @date    26-05-2018
 * @brief   
 *********************************************************************************************************************/

/* Includes ---------------------------------------------------------------------------------------------------------*/

#include "cli.h"
#include "nOS.h"
#include "stm32f0xx_hal.h"
#include "usbd_cdc_if.h"
#include "strfct.h"
#include "cli_menu.h"

/* Local Defines ----------------------------------------------------------------------------------------------------*/

#define CLI_STACK_SIZE      128 //512bytes
#define CLI_RXQ_SIZE        255
#define CLI_TXQ_SIZE        255
#define CLI_MAX_CMD_Q       3
#define CLI_MAX_CMD_SIZE    256

/* Local Typedefs ---------------------------------------------------------------------------------------------------*/

typedef struct{
    uint8_t cmd;
    uint8_t dataLen;
    uint8_t data[CLI_MAX_CMD_SIZE - 2];
}cmdLayerData_t;

typedef enum{
    CLI_MODE,
    DATA_MODE,
}cli_mode_e;
/* Forward Declarations ---------------------------------------------------------------------------------------------*/

static void Send_Prompt (char *menuStr);
static void CLI_Task    (void *arg);


size_t STR_vsnprintf(char* pOut, size_t Size, const char* pFormat, va_list va);

/* Local Constants --------------------------------------------------------------------------------------------------*/

/* Local Variables --------------------------------------------------------------------------------------------------*/

nOS_Thread  CLI_Thread;
nOS_Stack   CLI_Stack[CLI_STACK_SIZE];
nOS_Queue   CLI_RxQ;
nOS_Queue   CLI_TxQ;
nOS_Queue   CLI_CmdQ;
uint8_t     RxQ_Buff[CLI_RXQ_SIZE];
uint8_t     TxQ_Buff[CLI_TXQ_SIZE];
cmdLayerData_t RxCmd_Buff[CLI_MAX_CMD_Q];
uint8_t     CmdBuilderBuff[CLI_MAX_CMD_SIZE];
uint16_t    CmdBuilderBuffIdx;
uint8_t     TmpCmdBuff[CLI_MAX_CMD_SIZE];
cli_mode_e  cliMode;
char txByte;
/* Local Functions --------------------------------------------------------------------------------------------------*/

void CLI_Init(void)
{
    nOS_QueueCreate(&CLI_RxQ, RxQ_Buff, 1, CLI_RXQ_SIZE);
    nOS_QueueCreate(&CLI_TxQ, TxQ_Buff, 1, CLI_TXQ_SIZE);
    nOS_QueueCreate(&CLI_CmdQ, RxCmd_Buff, CLI_RXQ_SIZE, CLI_MAX_CMD_Q);
    nOS_ThreadCreate(&CLI_Thread, CLI_Task, NULL, CLI_Stack, CLI_STACK_SIZE, 1, "Console Task");
    cliMode = CLI_MODE;
    CmdBuilderBuffIdx = 0;
}

void CLI_Task(void *arg)
{
    uint8_t rxData;
    
    while(1)
    {
        // Parse incoming bytes
        if(!nOS_QueueIsEmpty(&CLI_RxQ))
        {
            nOS_QueueRead(&CLI_RxQ, &rxData, NOS_NO_WAIT);
            // Enter
            if (rxData == '\r')
            {
                nOS_QueueWrite(&CLI_CmdQ, CmdBuilderBuff, NOS_NO_WAIT);
                CmdBuilderBuffIdx = 0;
                CmdBuilderBuff[0] = 0;
                CLI_Send("\r\n", 2);
            }
            // Escape
            else if (rxData == '\e')
            {
                CLI_MENU_GoBack();
                CmdBuilderBuffIdx = 0;
                CmdBuilderBuff[0] = 0;
                // Send dummy char to print properly in console, this is because the \e received has
                // been echoed to the console
                CLI_Send("Q",1);
                CLI_Send("\r\n", 2);
                Send_Prompt(CLI_MENU_GetMenuStr());
            }
            // Backspace
            else if (rxData == 127)
            {
                if(CmdBuilderBuffIdx > 0)
                {
                    CmdBuilderBuffIdx--;
                    CmdBuilderBuff[CmdBuilderBuffIdx] = 0;
                }
            }
            else
            {
                CmdBuilderBuff[CmdBuilderBuffIdx] = rxData;
                CmdBuilderBuffIdx++;
                // Increment the null char every received byte
                CmdBuilderBuff[CmdBuilderBuffIdx] = 0;
            }
        }

        // Parse pending command
        if(!nOS_QueueIsEmpty(&CLI_CmdQ))
        {
            nOS_QueueRead(&CLI_CmdQ, TmpCmdBuff, NOS_NO_WAIT);
            CLI_MENU_CmdParse(TmpCmdBuff);
            
            Send_Prompt(CLI_MENU_GetMenuStr());
        }

        if(!nOS_QueueIsEmpty(&CLI_TxQ))
        {
            for (int i=0; i<nOS_QueueGetCount(&CLI_TxQ); i++)
            {
                nOS_QueueRead(&CLI_TxQ, &txByte, NOS_NO_WAIT);
                while (USBD_OK != CDC_Transmit_FS(&txByte, 1))
                {
                }
            }
        }

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        nOS_Sleep(20);
    }
}

// Wraper for the USB send command
void CLI_Send(char *Buf, uint16_t Len)
{
    for(int i=0; i<Len; i++)
    {
        nOS_QueueWrite(&CLI_TxQ, Buf+i, NOS_NO_WAIT);
    }
}

static void Send_Prompt(char *menuStr)
{
    char promptStr[32];

    sprintf(promptStr, "%s>", menuStr);
    CLI_Send(promptStr, strlen(promptStr));
}

// Rx from the USB CDC interrupt
void CLI_Rx(char *Buf, uint16_t Len)
{
    if(Len == 1)
    {
        nOS_QueueWrite(&CLI_RxQ, Buf, 2);
        CLI_Send(Buf, 1);
    }
    else
    {
        for(int i=0; i<Len; i++)
        {
            nOS_QueueWrite(&CLI_RxQ, Buf+i, 2);
            CLI_Send(Buf+i, 1);
        }
    }
}

void CLI_UserConnected()
{
	//CLI_Printf("User connected ...\r\n");
}

/* Global Functions -------------------------------------------------------------------------------------------------*/

/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Send formatted string to console if menu system is not active
  *
  * @param  Level       Level of printf logging
  * @param  pFormat     Formatted string
  * @param  ...         Parameter if any
  *
  * @retval None
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
#define CLI_PRINT_MAX_SIZE  64
char cliPrintBuff[CLI_PRINT_MAX_SIZE];

size_t CLI_Printf(const char* pFormat, ...)
{
    va_list          vaArg;
    char*            Buffer;
    size_t           Size;

    Buffer = cliPrintBuff;
    va_start(vaArg, (const char*)pFormat);
    Size = STR_vsnprintf(Buffer, CLI_PRINT_MAX_SIZE, pFormat, vaArg);
    CLI_Send(Buffer, Size);
    nOS_Sleep(2);
    va_end(vaArg);
    
    return Size;

}
