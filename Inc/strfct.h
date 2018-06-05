/**********************************************************************************************************************
 * @file    strfct.h
 * @author  Brioconcept Consulting, Inc.
 * @date    15-01-2016
 * @brief   String utility function
 *********************************************************************************************************************/

#ifndef __STRFCT_H__
#define __STRFCT_H__

/* Includes ---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

/* Global Defines ---------------------------------------------------------------------------------------------------*/

#define STR_MAX_PRINT_VALUE                 22
#define STR_USE_HEX_SUPPORT

/* Global Typedef ---------------------------------------------------------------------------------------------------*/

/* Global Variables -------------------------------------------------------------------------------------------------*/

/* Global Functions Prototypes --------------------------------------------------------------------------------------*/

size_t	    STR_snprintf		(char* pOut, size_t nSize, const char* pFormat, ...);
size_t	    STR_vsnprintf		(char* pOut, size_t nSize, const char* pFormat, va_list va);
size_t      STR_str2str     	(char* pOut, const char* pString);
uint8_t     STR_atoh8       	(char hi, char lo);
void        STR_h8toa       	(char* hi, char* lo, uint8_t Value);
void        STR_strnstrip   	(char*       pString, size_t nSize);
size_t      STR_strnlenstrip    (const char* pString, size_t nSize);
void        STR_strtrail        (char*       pString, size_t nSize);
size_t      STR_strnlen         (const char* pString, size_t nSize);
size_t      STR_memCount        (const uint8_t* pBuffer, uint8_t Code, size_t n);

/* ------------------------------------------------------------------------------------------------------------------*/

#endif //__STRFCT_H__
//EOF


