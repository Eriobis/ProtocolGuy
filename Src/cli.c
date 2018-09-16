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
#define CLI_HISTORY_SIZE    10  // 10 x CLI_MAX_CMD_SIZE

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
uint8_t     RxQ_Buff[CLI_RXQ_SIZE];
nOS_Queue   CLI_TxQ;
uint8_t     TxQ_Buff[CLI_TXQ_SIZE];
nOS_Queue   CLI_CmdQ;
cmdLayerData_t RxCmd_Buff[CLI_MAX_CMD_Q];
char     	CmdBuilderBuff[CLI_MAX_CMD_SIZE];
uint16_t    CmdBuilderBuffIdx;
uint8_t     HistoryBuff[CLI_HISTORY_SIZE][CLI_MAX_CMD_SIZE];
uint16_t    HistoryBuffCounter;
uint16_t    HistoryBuffPos;
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
    HistoryBuffCounter = 0;
    HistoryBuffPos = 0;
    cliMode = CLI_MODE;
    CmdBuilderBuffIdx = 0;
}

void CLI_Task(void *arg)
{
    bool charEscaped = false;
    bool specialCommand = false;
    uint8_t rxData;
    
    Send_Prompt(CLI_MENU_GetMenuStr());

    while(1)
    {
        // Parse incoming bytes
        if(!nOS_QueueIsEmpty(&CLI_RxQ))
        {
            nOS_QueueRead(&CLI_RxQ, &rxData, NOS_NO_WAIT);

            // Enter
            if (rxData == '\r')
            {
                // Do nothing if nothing has been written
                if(CmdBuilderBuffIdx > 0)
                {
                    nOS_QueueWrite(&CLI_CmdQ, CmdBuilderBuff, NOS_NO_WAIT);
                    memcpy(&HistoryBuff[HistoryBuffCounter], CmdBuilderBuff, CmdBuilderBuffIdx);
                    HistoryBuffCounter++;
                    HistoryBuffCounter %= CLI_HISTORY_SIZE;
                    CmdBuilderBuffIdx = 0;
                    memset(CmdBuilderBuff, 0, CLI_MAX_CMD_SIZE);
                    HistoryBuffPos = HistoryBuffCounter;

                    // Parse pending command
                    if(!nOS_QueueIsEmpty(&CLI_CmdQ))
                    {
                        nOS_QueueRead(&CLI_CmdQ, TmpCmdBuff, NOS_NO_WAIT);
                        CLI_MENU_CmdParse(TmpCmdBuff);
                        CLI_Send("\r\n", 2);
                        Send_Prompt(CLI_MENU_GetMenuStr());;
                    }
                }
                else
                {
                	CLI_Send("\r\n", 2);
                    Send_Prompt(CLI_MENU_GetMenuStr());
                }
            }
            // Escape
            else if (rxData == '\e')
            {
                if (!charEscaped)
                {
                    charEscaped = true;
                }
                else
                {
                	CLI_MENU_GoBack();
                    CLI_Send("\r\n", 2);
                    Send_Prompt(CLI_MENU_GetMenuStr());
                    CmdBuilderBuffIdx = 0;
                    CmdBuilderBuff[0] = 0;
                    charEscaped = false;
                }

            }
            // Backspace
            else if (rxData == 127)
            {
                if(CmdBuilderBuffIdx > 0)
                {
                    CmdBuilderBuffIdx--;
                    CmdBuilderBuff[CmdBuilderBuffIdx] = 0;
                }
                // Echo
                CLI_Send((char*)&rxData, 1);
            }
            else if(charEscaped)
            {
                if (rxData == '[')
                {
                    specialCommand = true;
                }
                else if(specialCommand)
                {
                    // Arrow up
                    if (rxData == 'A')
                    {
                        if((HistoryBuffPos-1) == HistoryBuffCounter)
                        {
                            CmdBuilderBuffIdx = 0;
                            memset(CmdBuilderBuff, 0, CLI_MAX_CMD_SIZE);
                        }
                        else
                        {
                            HistoryBuffPos --;
                            HistoryBuffPos %= CLI_HISTORY_SIZE;
                            memcpy(CmdBuilderBuff, &HistoryBuff[HistoryBuffPos], strlen((char*)&HistoryBuff[HistoryBuffPos]));
                            CmdBuilderBuffIdx = strlen((char*)CmdBuilderBuff);

                        }
                        CLI_Send("\r", 1);
                        Send_Prompt(CLI_MENU_GetMenuStr());
                        CLI_Send(CmdBuilderBuff, strlen(CmdBuilderBuff));

                    }
                    // Arrow down
                    else if (rxData == 'B')
                    {
                        if((HistoryBuffPos) == HistoryBuffCounter)
                        {
                        }
                        else
                        {
                            HistoryBuffPos ++;
                            HistoryBuffPos %= CLI_HISTORY_SIZE;
                            memcpy(CmdBuilderBuff, &HistoryBuff[HistoryBuffPos], strlen((char*)&HistoryBuff[HistoryBuffPos]));
                            CmdBuilderBuffIdx = strlen((char*)CmdBuilderBuff);
                        }
                        CLI_Send("\r", 1);
                        Send_Prompt(CLI_MENU_GetMenuStr());
                        CLI_Send(CmdBuilderBuff, strlen(CmdBuilderBuff));
                    }

                    specialCommand = false;
                    charEscaped = false;

                }
            }
            else
            {
                CmdBuilderBuff[CmdBuilderBuffIdx] = rxData;
                CmdBuilderBuffIdx++;
                // Increment the null char every received byte
                CmdBuilderBuff[CmdBuilderBuffIdx] = 0;
                // Echo
                CLI_Send((char*)&rxData, 1);
                specialCommand = false;
                charEscaped = false;
            }
        }

        if(!nOS_QueueIsEmpty(&CLI_TxQ))
        {
            for (int i=0; i<nOS_QueueGetCount(&CLI_TxQ); i++)
            {
                nOS_QueueRead(&CLI_TxQ, &txByte, NOS_NO_WAIT);
                while (USBD_OK != CDC_Transmit_FS((char*)&txByte, 1))
                {
                }
            }
        }

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        nOS_Sleep(10);
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
        // Echo, don't echo escape char
        // if(Buf != '\e')
        // {
        //     CLI_Send(Buf, 1);
        // }
    }
    else
    {
        for(int i=0; i<Len; i++)
        {
            nOS_QueueWrite(&CLI_RxQ, Buf+i, 2);
            // Echo, don't echo escape char
            // if(Buf+i != '\e')
            // {
            //     CLI_Send(Buf+i, 1);
            // }
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
