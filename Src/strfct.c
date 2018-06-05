/**********************************************************************************************************************
 * @file    strfct.c
 * @author  Brioconcept Consulting, Inc.
 * @date    15-01-2016
 * @brief   String functions
 *********************************************************************************************************************/

/* Includes ---------------------------------------------------------------------------------------------------------*/

#define STRFCT_GLOBAL
#include "strfct.h"
#undef STRFCT_GLOBAL
#include <ctype.h>
#include <string.h>

/* Local Defines ----------------------------------------------------------------------------------------------------*/

#define STR_OPT_PAD_ZERO            128	    // value in bit Position for padding with zero
#define STR_OPT_PAD_LEFT            64     	// value in bit Position for Padding flag
#define STR_OPT_LOWERCASE           32   	// value in bit Position for base
#define STR_OPT_SIGN_NEGATIVE       16     	// value in bit Position for sign
#define STR_OPT_BASE_HEXA           8       // value in bit Position for base

#define STR_putchar(s,c)            {*((char*)s) = (char)(c);}

/* Local Typedefs ---------------------------------------------------------------------------------------------------*/

typedef enum __STR_VarLength_e
{
    STR_VAR_16,
    STR_VAR_32,
    STR_VAR_64,
} STR_VarLength_e;


/* Forward Declarations ---------------------------------------------------------------------------------------------*/

static size_t STR_printi     (char* pOut, int64_t Value, size_t Width, uint8_t Option);
static size_t STR_prints     (char* pOut, char* pString,  size_t Width, uint8_t Option);

/* Local Constants --------------------------------------------------------------------------------------------------*/

/* Local Variables --------------------------------------------------------------------------------------------------*/

/* Local Functions --------------------------------------------------------------------------------------------------*/

/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Service function to print string for printf and sprintf
  *
  * @param  pOut            Pointer on output string
  * @param  pString         String to print
  * @param  Width           Width in the print string
  * @param  Option          Padding, Space or 0
  *
  * @retval size_t          Size of string
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static size_t STR_prints(char* pOut, char* pString, size_t Width, uint8_t Option)
{
    size_t  Counter;
    size_t  Len;
    char    PadChar;

    Counter = 0;
    PadChar = ' ';

    if(Width > 0)                                           // If different than '0' -> there is padding
    {
        Len = strlen(pString);

        // Check if length of the string is bigger or equal than the width of padding
        if(Len >= Width)               Width   = 0;         // No padding necessary, the number is bigger than the padding space
        else                           Width  -= Len;       // Remove unnecessary padding because of the length of the number
        if(Option & STR_OPT_PAD_ZERO)  PadChar = '0';       // If padding is zero, than set the variable

        if(Option & STR_OPT_PAD_LEFT)
        {
            for(; Width > 0; Width--)
            {
            	STR_putchar(&pOut[Counter], PadChar);
                Counter++;
            }
        }
    }

    Counter += STR_str2str(&pOut[Counter], pString);        // Print character until EOL

    for(; Width > 0; Width--)                               // Print padding character if any
    {
    	STR_putchar(&pOut[Counter], PadChar);
        Counter++;
    }

    return Counter;
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Service function to print string for printf and sprintf
  *
  * @param  pOut            Pointer on output string
  * @param  Value           Value to print
  * @param  Width           Width in the print string
  * @param  Option          Padding, Space or 0
  *
  * @retval uint8_t         Number of character printed
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
static size_t STR_printi(char* pOut, int64_t Value, size_t Width, uint8_t Option)
{
    size_t      Counter;
    uint64_t    _Value;
    char        PrintBuffer[STR_MAX_PRINT_VALUE];
    char*       pString;
    bool        Negative;
    char        Temp;
  #ifdef STR_USE_HEX_SUPPORT
    uint8_t     LetterBase;
    uint8_t     Base;
  #endif

    Counter    = 0;
    Negative   = false;
    _Value     = Value;

  #ifdef STR_USE_HEX_SUPPORT
    LetterBase = 'A' + (Option & STR_OPT_LOWERCASE);
    Base       = (Option & STR_OPT_BASE_HEXA) ? 16 : 10;
  #endif

    if(Value == 0)
    {
        PrintBuffer[0] = '0';
        PrintBuffer[1] = '\0';
        return STR_prints(pOut, PrintBuffer, Width, Option);
    }

  #ifdef STR_USE_HEX_SUPPORT
    if((Option & STR_OPT_SIGN_NEGATIVE) && (Base == 10) && (Value < 0))
  #else
    if((Option & STR_OPT_SIGN_NEGATIVE) && (Value < 0))
  #endif
    {
        Negative = true;
        _Value   = -Value;
    }

    pString = PrintBuffer + (STR_MAX_PRINT_VALUE - 1);
    *pString = '\0';

    while(_Value)
    {
      #ifdef STR_USE_HEX_SUPPORT

        Temp = _Value % Base;

        if(Temp >= 10)
        {
            Temp += (LetterBase -'0' - 10);
        }

        *--pString = Temp + '0';
        _Value = _Value / Base;

      #else

        Temp = _Value % 10;
        *--pString = Temp + '0';
        _Value = _Value / 10;

      #endif
    }

    if(Negative == true)
    {
        if((Width != 0) && (Option & STR_OPT_PAD_ZERO))
        {
            STR_putchar(pOut, '-');
            Counter++;
            Width--;
        }
        else
        {
            *--pString = '-';
        }
    }

    return Counter + STR_prints(pOut, pString, Width, Option);
}


/* Global Functions -------------------------------------------------------------------------------------------------*/

/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Print a formatted text in a buffer from ...
  *
  * @param  pOut            Pointer on output string
  * @param  Size            Maximum character to print
  * @param  pFormat         Formatted string
  * @param  ...             variable list of argument
  *
  * @note           Will support the following Parameter
  *
  *                  %s               Print a string
  *                  %0               pad with zero
  *                  %1 to %255       number of padding space
  *                  %01 to %0255     number of padding 0
  *                  %c               special character
  *                  %l               for long support                  ( valid only for d,u,x,X )
  *                  %ll              for longlong (64 Bits) support    ( valid only for d,u,x,X )
  *                  %u               unsigned integer
  *                  %d               integer
  *                  %x               hexadecimal in lower case
  *                  %X               hexadecimal in upper case
  *
  *
  *                   note: this is the order in which they must be use
  *
  *                   ex.  %05lX   pad with ZERO +
  *                                5 padding character +
  *                                it will be a long +
  *                                it's a HEX printed in uppercase
  *
  *                   ex. %6ld     normal padding ( no Parameter ) +
  *                                padding with space ( no Parameter ) +
  *                                6  padding space on the left +
  *                                it will be a long +
  *                                it is a decimal value
  *
  *
  * @retval uint8_t         Number of character printed
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
size_t STR_snprintf(char* pOut, size_t Size, const char* pFormat, ...)
{
    va_list vaArg;

    va_start(vaArg, (const char*)pFormat);
    Size = STR_vsnprintf(pOut, Size, pFormat, vaArg);
    va_end(vaArg);

    return Size;
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Print a formatted text in a buffer from a va list of argument
  *
  * @param  pOut            Pointer on output string
   * @param  Size            Maximum character to print
  * @param  pFormat         Formatted string
  * @param  va              list of argument
  *
  * @note           Will support the following Parameter
  *
  *                  %%               Print symbol %
  *                  %s               Print a string
  *                  %0               pad with zero
  *                  %1 to %255       number of padding space
  *                  %01 to %0255     number of padding 0
  *                  %c               special character
  *                  %l               for long     (32 Bits) support        ( valid only for d,u,x,X )  Ex. %llu
  *                  %ll              for longlong (64 Bits) support        ( valid only for d,u,x,X )
  *                  %u               unsigned integer
  *                  %d               integer
  *                  %x               hexadecimal in lower case
  *                  %X               hexadecimal in upper case
  *
  *
  *                   note: this is the order in which they must be use
  *
  *                   ex.  %05lX   pad with ZERO +
  *                                5 padding character +
  *                                it will be a long +
  *                                it's a HEX printed in uppercase
  *
  *                   ex. %6ld     normal padding ( no Parameter ) +
  *                                padding with space ( no Parameter ) +
  *                                6  padding space on the left +
  *                                it will be a long +
  *                                it is a decimal value
  *
  *
  * @retval uint8_t         Number of character printed
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
size_t STR_vsnprintf(char* pOut, size_t Size, const char* pFormat, va_list va)
{
    uint32_t            Counter;
    size_t              Width;
    STR_VarLength_e     SizeVar;
    int64_t             n;
    uint8_t             Option;
    char*               s;
    char                scr[2];

    Counter = 0;

    for(; (*pFormat != '\0') && (Counter < Size); pFormat++)
    {
        if(*pFormat == '%')
        {
            pFormat++;
            Width   = 0;
            Option  = STR_OPT_PAD_LEFT;
            SizeVar = STR_VAR_16;

            if(*pFormat == 's')
            {
                s = va_arg(va, char *);
                Counter += STR_prints(&pOut[Counter], s ? s : "(null)", Width, Option);
                continue;
            }

            if(*pFormat == 'c')
            {
                scr[0] = (uint8_t)va_arg(va, int);
                scr[1] = '\0';
                Counter += STR_prints(&pOut[Counter], scr, Width, Option);
                continue;
            }

            while(*pFormat == '0')
            {
                pFormat++;
                Option |= STR_OPT_PAD_ZERO;
            }

            for(; (*pFormat >= '0') && (*pFormat <= '9'); pFormat++)
            {
                Width *= 10;
                Width += *pFormat - '0';
            }

            if(*pFormat == 'l')
            {
                SizeVar = STR_VAR_32;
                pFormat++;
                if(*pFormat == 'l')
                {
                    SizeVar = STR_VAR_64;
                    pFormat++;
                }
            }

            if(*pFormat == 'd')
            {
                switch(SizeVar)
                {
                    case STR_VAR_16: n = (int16_t)va_arg(va, int32_t);    break;
                    case STR_VAR_32: n = (int32_t)va_arg(va, int32_t);    break;
                    case STR_VAR_64: n = (int64_t)va_arg(va, int64_t);    break;
                }
                Counter += STR_printi(&pOut[Counter], n, Width, Option | STR_OPT_SIGN_NEGATIVE | STR_OPT_LOWERCASE);
                continue;
            }

            switch(SizeVar)
            {
                case STR_VAR_16: n = (uint16_t)va_arg(va, uint32_t);    break;
                case STR_VAR_32: n = (uint32_t)va_arg(va, uint32_t);    break;
                case STR_VAR_64: n = (uint64_t)va_arg(va, uint64_t);    break;
            }

          #ifdef STR_USE_HEX_SUPPORT
            if(*pFormat == 'x')
            {
                Counter += STR_printi(&pOut[Counter], n, Width, Option | STR_OPT_BASE_HEXA | STR_OPT_LOWERCASE);
                continue;
            }

            if(*pFormat == 'X')
            {
                Counter += STR_printi(&pOut[Counter], n, Width, Option | STR_OPT_BASE_HEXA);
                continue;
            }
          #endif

            if(*pFormat == 'u')
            {
                Counter += STR_printi(&pOut[Counter], n, Width, Option | STR_OPT_LOWERCASE);
                continue;
            }
        }
        else
        {
        	STR_putchar(&pOut[Counter], *pFormat);
            Counter++;
        }
    }

    STR_putchar(&pOut[Counter], '\0');

    return Counter;
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Transfer the string to the other string until EOL
  *
  * @param  pOut            Pointer on output string
  * @param  pFormat         Value to print
  *
  * @retval uint8_t         Number of character printed
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
size_t STR_str2str(char* pOut, const char* pString)
{
    size_t Counter = 0;

    for(; *pString; pString++)                                  // Print character until EOL
    {
    	STR_putchar(&pOut[Counter], (char)*pString);
        Counter++;
    }
    STR_putchar(&pOut[Counter], '\0');

    return Counter;
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  get uint8_t from 2 ASCII
  *
  * @param  hi
  * @param  lo
  *
  * @retval uint8_t
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
uint8_t STR_atoh8(char hi, char lo)
{
    uint8_t b;

    hi = toupper(hi);
    if( isxdigit(hi) ) {
        if( hi > '9' ) {
            hi -= 7;      // software offset for A-F
        }
        hi -= 0x30;                  // subtract ASCII offset
        b = hi<<4;
        lo = toupper(lo);
        if( isxdigit(lo) ) {
            if( lo > '9' ) {
                lo -= 7;  // software offset for A-F
            }
            lo -= 0x30;              // subtract ASCII offset
            b = b + lo;
            return b;
        } // else error
    }  // else error
    return 0;
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  get 2 ASCII for an uint8_t
  *
  * @param  pointer to hi
  * @param  pointer to lo
  * @param  value to convert to 2 ASCII
  *
  * @retval uint8_t
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
void STR_h8toa(char* hi, char* lo, uint8_t Value)
{
    *hi = (Value >> 4)   + (((Value >> 4)   > 9 ) ? ('A' - 10) : '0');
    *lo = (Value & 0x0F) + (((Value & 0x0F) > 9 ) ? ('A' - 10) : '0');
}

/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Strip the trailing space starting at nSize
  *
  * @param  char*   pString
  * @param  size_t  nSize
  *
  * @retval None
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
void STR_strnstrip(char* pString, size_t nSize)
{
    char* End;

    End = pString + (nSize - 1);
    while((End >= pString))
    {
        if(*End == ' ')
        {
            *End = '\0';
        }
        else
        {
            return;
        }
        End--;
    }
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  add trailing space up to the end of the string
  *
  * @param  char*       pString
  * @param  size_t      nSize
  *
  * @retval None
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
void STR_strtrail(char* pString, size_t nSize)
{
    bool FoundEnd = false;

    for(int i = 0; i < nSize; i++)
    {
        if((*((char*)pString + i) == '\0') || (FoundEnd == true))
        {
            FoundEnd = true;
            *((char*)pString + i) = ' ';
        }
    }
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Return length without counting trailing space starting at nSize
  *
  * @param  const char* pString
  * @param  size_t      nSize
  *
  * @retval size_t      Size of the string minus training space
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
size_t STR_strnlenstrip(const char* pString, size_t nSize)
{
    char* End;

    End = (char*)pString + (nSize - 1);
    while((End >= (char*)pString) && (*End == ' '))
    {
        End--;
        nSize--;
    }

    return nSize;
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Function to return length of a string, or Max if reach
  *
  * @param  pString         String to calculate the length
  * @param  nSize           Size Max to return in case it is longer
  *
  * @retval size_t          Size of the string
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
size_t STR_strnlen(const char* pString, size_t nSize)
{
   size_t  	    Counter = 0;
   char*   		pPtr;

	pPtr = (char*)pString;

	while((*pPtr != (char)'\0') && (Counter <= nSize))
	{
		pPtr++;
		Counter++;
	}

	return Counter;
}


/**
  *--------------------------------------------------------------------------------------------------------------------
  *
  * @brief  Count number of occurrence of a code in memory region
  *
  * @param  pBuffer         Buffer address
  * @param  Code            Code to count
  * @param  n               Count up to n
  *
  * @retval size_t          number of occurrence
  *
  *--------------------------------------------------------------------------------------------------------------------
  */
size_t STR_memCount(const uint8_t* pBuffer, uint8_t Code, size_t n)
{
    size_t Count = 0;

    for(size_t i = 0; i < n ; i++)
    {
        if(pBuffer[i] == Code)
        {
            Count++;
        }
    }

    return Count;
}


/* --------------------------------------------------------------------------------------------------------------------*/

