/**********************************************************************************************************************
 * @file    cli_menu.c
 * @author  Brioconcept Consulting, Inc.
 * @date    20-09-2017
 * @brief   
 **********************************************************************************************************************

/* Includes ---------------------------------------------------------------------------------------------------------*/

#include "cli_menu.h"

/* Local Defines ----------------------------------------------------------------------------------------------------*/
//          Command     Callback

#define X_COMMAND_ARRAY \
X_CLI_CMD( HELP_CMD,  "help",    ShowHelp     )\
X_CLI_CMD( I2C_CMD,   "I2C",     NULL         )\
X_CLI_CMD( I2C_CMD_lc,"i2c",     NULL         )\
X_CLI_CMD( SPI_CMD_lc,"spi",     NULL         )\
X_CLI_CMD( SPI_CMD,   "SPI",     NULL         )

#define X_I2C_CMD_ARRAY \
X_CLI_I2C_CMD( I2C_ADDR_CMD,        "addr",     NULL     )\
X_CLI_I2C_CMD( I2C_WRITE_CMD,       "w",        NULL     )\
X_CLI_I2C_CMD( I2C_WRITE_READ_CMD,  "wr",       NULL     )\
X_CLI_I2C_CMD( I2C_READ_CMD,        "r",        NULL     )\
X_CLI_I2C_CMD( I2C_SCAN,            "scan",     NULL     )

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

/* Forward Declarations ---------------------------------------------------------------------------------------------*/

static void ParseMainCmd    (uint8_t *cmd);
static void ParseConfigCmd  (uint8_t *cmd);
static void ParseUartCmd    (uint8_t *cmd);
static void ParseSpiCmd     (uint8_t *cmd);
static void ParseI2CCmd     (uint8_t *cmd);
static void ParseCANCmd     (uint8_t *cmd);
static void GotoMenu        (CLI_MENU_PAGE_e page);
static void ShowHelp		(void);

/* Local Constants --------------------------------------------------------------------------------------------------*/

const char* menuStr[] = {"main", "config", "UART", "I2C", "SPI", "CAN", "ECHO"};

#define X_CLI_CMD( IDX, COMMAND, CALLBACK )  COMMAND,
const char* cmdStr[] = { X_COMMAND_ARRAY };
#undef X_CLI_CMD

#define X_CLI_CMD( IDX, COMMAND, CALLBACK )  CALLBACK,
void (*cmdCallback[])(void) = { X_COMMAND_ARRAY };
#undef X_CLI_CMD


/* Local Variables --------------------------------------------------------------------------------------------------*/

CLI_MENU_PAGE_e ActualPage = MENU_MAIN;
CLI_MENU_PAGE_e PreviousPage = MENU_MAIN;

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
    if(strlen(cmd) == 1)
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
        for(int i=0; i< NUM_OF_CLI_CMD; i++)
        {
            if (!strcmp(cmd, cmdStr[i]))
            {
                CLI_Printf("Command OK");
                cmdCallback[i]();
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
    CLI_Printf("----- HELP -----\r\n");
    CLI_Printf("Known commands : \r\n");
    for(int i=0; i< NUM_OF_CLI_CMD; i++)
    {
        CLI_Printf(" - %s",cmdStr[i]);

    }
    CLI_Printf("----------------\r\n");
    CLI_Printf("Help !!!!\r\n");
    CLI_Printf("Help !!!!\r\n");
    CLI_Printf("Help !!!!\r\n");
    CLI_Printf("Help !!!!\r\n");
    CLI_Printf("Help !!!!\r\n");
    CLI_Printf("Help !!!!\r\n");
    CLI_Printf("Help !!!!\r\n");
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

uint8_t* CLI_MENU_GetMenuStr (void)
{
    return menuStr[ActualPage];
}

void CLI_MENU_GoBack(void)
{
    GotoMenu(PreviousPage);
}
