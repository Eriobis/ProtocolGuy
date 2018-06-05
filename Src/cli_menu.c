/**********************************************************************************************************************
 * @file    cli_menu.c
 * @author  Brioconcept Consulting, Inc.
 * @date    20-09-2017
 * @brief   
 *********************************************************************************************************************/

/* Includes ---------------------------------------------------------------------------------------------------------*/

#include <string.h>
#include <stdarg.h>
#include "cli_menu.h"
#include "i2c.h"
#include "cli.h"
#include "strfct.h"

/* Local Defines ----------------------------------------------------------------------------------------------------*/
//          Command     Callback

#define X_COMMAND_ARRAY \
X_CLI_CMD( HELP_CMD,  "help",    ShowHelp     )\
X_CLI_CMD( I2C_CMD,   "I2C",     NULL         )\
X_CLI_CMD( I2C_CMD_lc,"i2c",     NULL         )\
X_CLI_CMD( SPI_CMD_lc,"spi",     NULL         )\
X_CLI_CMD( SPI_CMD,   "SPI",     NULL         )

#define X_I2C_CMD_ARRAY \
X_CLI_I2C_CMD( I2C_ADDR_CMD,        "addr",     CLI_I2C_SetAddr         )\
X_CLI_I2C_CMD( I2C_WRITE_CMD,       "w",        CLI_I2C_WriteCmd        )\
X_CLI_I2C_CMD( I2C_WRITE_READ_CMD,  "wr",       CLI_I2C_WriteReadCmd    )\
X_CLI_I2C_CMD( I2C_READ_CMD,        "r",        NULL                    )\
X_CLI_I2C_CMD( I2C_HELP_CMD,        "h",        ShowI2CHelp             )\
X_CLI_I2C_CMD( I2C_SCAN,            "scan",     NULL                    )

#define X_MENU_COMMAND_ARRAY \
X_CLI_MENU_CMD( HELP_MENU  "h",     MENU_HELP   )\
X_CLI_MENU_CMD( I2C_MENU   "i",     MENU_I2C    )\
X_CLI_MENU_CMD( SPI_CMD,   "s",     MENU_SPI    )\
X_CLI_MENU_CMD( UART_CMD,  "u",     MENU_UART   )\
X_CLI_MENU_CMD( CAN_CMD,   "c",     MENU_CAN    )

/* Local Typedefs ---------------------------------------------------------------------------------------------------*/

typedef enum __CLI_MENU_PAGE_e
{
    MENU_MAIN,
    MENU_CONFIG,
    MENU_UART,
    MENU_I2C,
    MENU_SPI,
    MENU_CAN,
    MENU_HELP,
    MENU_ECHO
}CLI_MENU_PAGE_e;

typedef enum
{
#define X_CLI_CMD(IDX, CMD, CALLBACK) IDX,
    X_COMMAND_ARRAY
#undef X_CLI_CMD
    NUM_OF_CLI_CMD
}CLI_CmdIndex_e;

typedef enum
{
#define X_CLI_I2C_CMD(IDX, CMD, CALLBACK) IDX,
    X_I2C_CMD_ARRAY
#undef X_CLI_I2C_CMD
    NUM_OF_I2C_CLI_CMD
}CLI_I2CCmdIndex_e;

/* Forward Declarations ---------------------------------------------------------------------------------------------*/
// Parser Section
static void ParseMainCmd    (uint8_t *cmd);
static void ParseConfigCmd  (uint8_t *cmd);
static void ParseUartCmd    (uint8_t *cmd);
static void ParseSpiCmd     (uint8_t *cmd);
static void ParseI2CCmd     (uint8_t *cmd);
static void ParseCANCmd     (uint8_t *cmd);
static void GotoMenu        (CLI_MENU_PAGE_e page);

//I2C Section
static void CLI_I2C_WriteCmd        (uint8_t *arg);
static void CLI_I2C_WriteReadCmd    (uint8_t *arg);
static void CLI_I2C_SetAddr         (uint8_t *arg);

// Help section
static void ShowHelp        (void);
static void ShowI2CHelp		(uint8_t *arg);

/* Local Constants --------------------------------------------------------------------------------------------------*/

volatile char i2cAddrStr[7] = "I2C@00";
char* menuStr[] = {"main", "config", "UART", i2cAddrStr, "SPI", "CAN", "ECHO"};


// Main Cmd
#define X_CLI_CMD( IDX, COMMAND, CALLBACK )  COMMAND,
const char* mainCmdArray[] = { X_COMMAND_ARRAY };
#undef X_CLI_CMD
//Main Cmd Callback
#define X_CLI_CMD( IDX, COMMAND, CALLBACK )  CALLBACK,
void (*mainCmdCallback[])(void) = { X_COMMAND_ARRAY };
#undef X_CLI_CMD

//I2C Commands
#define X_CLI_I2C_CMD( IDX, COMMAND, CALLBACK )  COMMAND,
const char* I2CCmdArray[] = { X_I2C_CMD_ARRAY };
#undef X_CLI_I2C_CMD
//I2C Commands Callback
#define X_CLI_I2C_CMD( IDX, COMMAND, CALLBACK )  CALLBACK,
void (*I2CCmdCallback[])(uint8_t *arg) = { X_I2C_CMD_ARRAY };
#undef X_CLI_I2C_CMD

/* Local Variables --------------------------------------------------------------------------------------------------*/

CLI_MENU_PAGE_e ActualPage = MENU_MAIN;
CLI_MENU_PAGE_e PreviousPage = MENU_MAIN;
uint8_t dataCommand[64];
uint8_t dataCommandIdx;

/* Local Functions --------------------------------------------------------------------------------------------------*/

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void ParseMainCmd(uint8_t *cmd)
{
    uint8_t command;
    // One char command
    if(strlen((char*)cmd) == 1)
    {
        command = cmd[0];

        switch (command)
        {
            case 'i' :
                GotoMenu(MENU_I2C);
                break;
            case 'u' :
                GotoMenu(MENU_UART);
                break;
            case 'c' :
                GotoMenu(MENU_CAN);
                break;
            case 'h' :
            case '?' :
                ShowHelp();
                break;
            case 's' :
                GotoMenu(MENU_SPI);
                break;
            case 'e' :
                GotoMenu(MENU_ECHO);
                break;
            default:
                break;
        }
    }
    else
    {
        for(CLI_CmdIndex_e i=0; i< NUM_OF_CLI_CMD; i++)
        {
            if (!strcmp((char*)cmd, mainCmdArray[i]))
            {
                mainCmdCallback[i]();
            }
        }
    }
}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void ParseConfigCmd(uint8_t *cmd)
{

}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void ParseUartCmd(uint8_t *cmd)
{

}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void ParseSpiCmd(uint8_t *cmd)
{

}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void ParseI2CCmd(uint8_t *cmd)
{
    char *argPtr = NULL;
    char *CmdPtr = NULL;
    CmdPtr = strtok((char*)cmd, "=");
    for(CLI_I2CCmdIndex_e i=0; i< NUM_OF_I2C_CLI_CMD; i++)
    {
        if (!strcmp(CmdPtr, I2CCmdArray[i]))
        {
            // Find the argument pointer
            argPtr = strtok(NULL, ";");
            if(argPtr != NULL)
            {
                I2CCmdCallback[i](argPtr);
            }
        }
    }
}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void CLI_I2C_WriteCmd(uint8_t *arg)
{
    uint8_t dataLen;
    CLI_Printf("I2C W Cmd ...\r\n");
    dataLen = parseDataStr(arg);
    I2C_Cmd_Write(dataCommand, dataLen);
}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void CLI_I2C_WriteReadCmd(uint8_t *arg)
{
    uint8_t dataLen;
    CLI_Printf("I2C WR Cmd ...\r\n");
    dataLen = parseDataStr(arg);
    I2C_Cmd_Write_Read(dataCommand, dataLen);
}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void CLI_I2C_SetAddr(uint8_t *arg)
{
    uint8_t dataLen;
    char addrStr[6];
    CLI_Printf("I2C Addr Cmd ...\r\n");
    dataLen = parseDataStr(arg);
    I2C_SetAddress(dataCommand[0]);
    snprintf(menuStr[3], 7, "I2C@%02X", dataCommand[0]);
}

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static void ParseCANCmd(uint8_t *cmd)
{

}

static void GotoMenu(CLI_MENU_PAGE_e page)
{
    if (page != PreviousPage)
    {
        PreviousPage = ActualPage;
    }
    
    ActualPage = page;
}

static void ShowHelp(void)
{
    char commandStr[16];

    CLI_Printf("----- HELP -----\r\n");
    CLI_Printf("Known commands :");
    for(int i=0; i< NUM_OF_CLI_CMD; i++)
    {
        sprintf(commandStr, "%s : ", mainCmdArray[i]);
        CLI_Printf(commandStr);
    }
    ShowI2CHelp(NULL);
    CLI_Printf("----------------\r\n");
}

static void ShowI2CHelp(uint8_t *arg)
{
    char commandStr[16];
    CLI_Printf("\r\n-- I2C Section --");
    for(int i=0; i< NUM_OF_I2C_CLI_CMD; i++)
    {
        sprintf(commandStr, "%s : ", I2CCmdArray[i]);
        CLI_Printf(commandStr);
    }
}

/* Global Functions -------------------------------------------------------------------------------------------------*/

/**
  *--------------------------------------------------------------------------------------------------------------------
  * @brief  
  *
  * @param  none
  *
  * @retval none
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
void CLI_MENU_CmdParse(uint8_t *cmd)
{
    switch(ActualPage)
    {
        case MENU_MAIN:
            ParseMainCmd(cmd);
            break;
        case MENU_CONFIG:
            ParseConfigCmd(cmd);
            break;
        case MENU_UART:
            ParseUartCmd(cmd);
            break;
        case MENU_I2C:
            ParseI2CCmd(cmd);
            break;
        case MENU_SPI:
            ParseSpiCmd(cmd);
            break;
        case MENU_CAN:
            ParseCANCmd(cmd);
            break;
        case MENU_ECHO:
            
            break;
        case MENU_HELP:

             break;
    }

}

char* CLI_MENU_GetMenuStr (void)
{
    return menuStr[ActualPage];
}

void CLI_MENU_GoBack(void)
{
    GotoMenu(PreviousPage);
}

typedef enum __Data_Type_e
{
    DATA_0X,
    DATA_HEX,
}Data_Type_e;

uint8_t parseDataStr(char *str)
{
    uint8_t len;
    char *strPtr;
    strPtr = str;
    
    dataCommandIdx = 0;
    len = 0;
    while(strPtr < (str+strlen(str)))
    {
        if((*strPtr == ' ') || (*strPtr == ';'))
        {
            strPtr++;
            len++;
        }
        if((*strPtr == '0' && *(strPtr+1) == 'x') || (*strPtr == '0' && *(strPtr+1) == 'X'))
        {
            strPtr += 2;
        }
        dataCommand[dataCommandIdx] = STR_atoh8(*strPtr, *(strPtr+1));
        dataCommandIdx++;
        len++;
        strPtr += 2;
    }
    for(int i=0; i< dataCommandIdx; i++)
    {
        CLI_Printf("%02x", dataCommand[i]);
    }
    CLI_Printf("\r\n");
    return len;
}
