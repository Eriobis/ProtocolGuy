/**********************************************************************************************************************
 * @file    cli.c
 * @author  Simon Benoit
 * @date    26-05-2018
 * @brief   
 **********************************************************************************************************************

/* Includes ---------------------------------------------------------------------------------------------------------*/

#include "cli.h"
#include "nOS.h"
#include "stm32f0xx_hal.h"

/* Local Defines ----------------------------------------------------------------------------------------------------*/

#define CLI_STACK_SIZE  64

/* Local Typedefs ---------------------------------------------------------------------------------------------------*/

/* Forward Declarations ---------------------------------------------------------------------------------------------*/

/* Local Constants --------------------------------------------------------------------------------------------------*/

nOS_Thread CLI_Thread;
nOS_Stack  CLI_Stack[CLI_STACK_SIZE];

/* Local Variables --------------------------------------------------------------------------------------------------*/

void CLI_Task(void *arg);

/* Local Functions --------------------------------------------------------------------------------------------------*/

void CLI_Init(void)
{
    nOS_ThreadCreate(&CLI_Thread, CLI_Task, NULL, CLI_Stack, CLI_STACK_SIZE, 1, "Console Task");
}

void CLI_Task(void *arg)
{

    while(1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        nOS_Sleep(100);
    }
}

/* Global Functions -------------------------------------------------------------------------------------------------*/
