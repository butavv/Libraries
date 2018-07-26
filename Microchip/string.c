//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: string.c
//
//  Description:
//  This file contains functions to read and write numeric ASCII strings.
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#include "string.h"

//Function: usitoa
//Inputs:
//  val:        Value to convert, range 0-65535
//  n:          Number of bytes written into *str, range 1-5
//  *str:       Location to save resulting string: SIZE WILL BE n BYTES,0-PADDED
//  pad:        Pad output? (TRUE/FALSE)
//  padChar:    Character to pad with
//Outputs: UINT8 nb - number of bytes written to str
//
//Description: "Unsigned Int to ASCII - 1-5-character"
//Converts an unsigned integer, range 0-65535, to an ascii number string with
//'0'-padding to n characters (1-5) if requested (pad = TRUE)
UINT8 usitoa( UINT16 val, char str[], UINT8 n, BOOL pad, UINT8 padChar )
{
    UINT16 pow, maxVal;
    INT16 i;
    BOOL outputValid = TRUE;
    BOOL firstVal = TRUE;
    UINT8 res[5];
    UINT8 nb = 0;
    UINT8 nv = 0;

    if (n > 5)
    {
        n = 5;
    }
    switch (n)
    {
        case 0:
            outputValid = FALSE;
            break;
        case 1:
            maxVal = 9;
            break;
        case 2:
            maxVal = 99;
            break;
        case 3:
            maxVal = 999;
            break;
        case 4:
            maxVal = 9999;
            break;
        case 5:
            maxVal = 65535;
            break;
    }
    if (val > maxVal)
    {
        outputValid = FALSE;
    }
    if (TRUE == outputValid)
    {
        //Load initial character value to output
        for (i=0; i<n; i++)
        {
            res[i] = '0';
        }

        for (i=(5-n); i<5; i++)
        {
            switch(i)
            {
                case 0:
                    pow=10000;
                    break;
                case 1:
                    pow=1000;
                    break;
                case 2:
                    pow=100;
                    break;
                case 3:
                    pow=10;
                    break;
                case 4:
                    pow=1;
                    break;
            }
            if ((TRUE == firstVal) && (val >= pow))
            {
                firstVal = FALSE;
                nb = 5-i;
            }
            while( val >= pow )
            {
                val -= pow;
                res[i+n-5]++;
            }
        }
        //Copy over temp string to proper location
        nv = nb;
        if (TRUE == pad)
        {
            nb = n;
        }
        for (i=0; i<nb; i++)
        {
            if ((TRUE == pad) && ((nb-nv) > i))
            {
                str[i] = padChar;
            }
            else
            {
                str[i] = res[i+n-nb];
            }
        }
        if (0 == nb)
        {
            str[0] = '0';
            str[1] = 0;     //Print a single 0 output
        }
        else if (nb < n)
        {
            str[nb] = 0;    //Null-terminate early
        }
    }
    else
    {   //Value out of range: print out xxxx
        for (i=0; i<n; i++)
        {
            str[i] = 'x';
            nb = n;
        }
    }

    return nb;
}

//Function: usitoa4
//Inputs:
//  val:        Value to convert, range 0-9999
//  *str:       Location to save resulting string: SIZE WILL BE 4 BYTES, 0-PADDED
//Outputs: none
//
//Description: "Unsigned Int to ASCII - 4-character"
//Converts an unsigned integer, range 0-9999, to an ascii number string with
//'0'-padding to 4 characters
void usitoa4( UINT16 val, char *str )
{
    UINT16 pow;
    INT16 i;
    UINT8 firstVal = 0;

    if (val > 9999)
    {
        for (i=0; i<4; i++)
        {
            str[i] = 'x';   //Out of range
        }
        return;
    }

    //Load padding characters into output
    for (i=0; i<4; i++)
    {
        str[i] = '0';
    }

    for (i=0; i<4; i++)
    {
        switch(i)
        {
            case 0:
                pow=1000;
                break;
            case 1:
                pow=100;
                break;
            case 2:
                pow=10;
                break;
            case 3:
                pow=1;
                break;
        }
        if (val >= pow)
        {
            firstVal = 1;
        }
        if (firstVal !=0)
        {
            str[i] = '0';
        }
        while( val >= pow )
        {
            val -= pow;
            str[i]++;
        }
    }
}

//Function: usitoa5
//Inputs:
//  val:        Value to convert, range 0-65535
//  *str:       Location to save resulting string: SIZE WILL BE 5 BYTES, 0-PADDED
//Outputs: none
//
//Description: "Unsigned Int to ASCII - 5-character"
//Converts an unsigned integer, range 0-65535, to an ascii number string with
//'0'-padding to 5 characters
void usitoa5( UINT16 val, char *str )
{
    UINT16 pow;
    INT16 i;
    UINT8 firstVal = 0;

    //Load padding characters into output
    for (i=0; i<5; i++)
    {
        str[i] = '0';
    }

    for (i=0; i<5; i++)
    {
        switch(i)
        {
            case 0:
                pow=10000;
                break;
            case 1:
                pow=1000;
                break;
            case 2:
                pow=100;
                break;
            case 3:
                pow=10;
                break;
            case 4:
                pow=1;
                break;
        }
        if (val >= pow)
        {
            firstVal = 1;
        }
        if (firstVal !=0)
        {
            str[i] = '0';
        }
        while( val >= pow )
        {
            val -= pow;
            str[i]++;
        }
    }
}

//Function: usitoa8
//Inputs:
//  val:        Value to convert, range 0-99999999
//  *str:       Location to save resulting string: SIZE WILL BE 8 BYTES, 0-PADDED
//Outputs: none
//
//Description: "Unsigned Int to ASCII - 8-character"
//Converts an unsigned integer, range 0-99999999, to an ascii number string with
//'0'-padding to 8 characters
void usitoa8( UINT32 val, char *str )
{
    UINT32 pow;
    INT32 i;
    UINT32 firstVal = 0;

    //Load padding characters into output
    for (i=0; i<8; i++)
    {
        str[i] = '0';
    }

    for (i=0; i<8; i++)
    {
        switch(i)
        {
            case 0:
                pow=10000000;
                break;
            case 1:
                pow=1000000;
                break;
            case 2:
                pow=100000;
                break;
            case 3:
                pow=10000;
                break;
            case 4:
                pow=1000;
                break;
            case 5:
                pow=100;
                break;
            case 6:
                pow=10;
                break;
            case 7:
                pow=1;
                break;
        }
        if (val >= pow)
        {
            firstVal = 1;
        }
        if (firstVal !=0)
        {
            str[i] = '0';
        }
        while( val >= pow )
        {
            val -= pow;
            str[i]++;
        }
    }
}

//Function: atousi4
//Inputs:
//  *str:   Pointer to 4-character numeric string.  EX: "0052" "7000" "8553"
//Outputs: none
//
//Description:
//Converts an unsigned ascii number string to unsigned integer, range 0-9999
UINT16 atousi4( char *str )
{
    UINT16 sum, pow;
    INT16 i;

    pow = 1;
    sum = 0;

    for (i = 3; i>=0; i--)
    {
        sum += ((str[i]-'0') * pow);
        pow *= 10;
    }

    return(sum);
}

//Function: atousi5
//Inputs:
//  *str:   Pointer to 5-character numeric string.  EX: "00052" "45000" "01234"
//Outputs: UINT16 result
//
//Description:
//Converts an unsigned ascii number string to unsigned integer, range 0-65535
UINT16 atousi5( char *str )
{
    UINT16 sum, pow;
    INT16 i;

    pow = 1;
    sum = 0;

    for (i = 4; i>=0; i--)
    {
        sum += ((str[i]-'0') * pow);
        pow *= 10;
    }

    return(sum);
}

//Function: atousi8
//Inputs:
//  *str:   Pointer to 8-character numeric string.  EX: "00001234"
//Outputs: UINT32 result
//
//Description:
//Converts an unsigned ascii number string to UINT32, range 0-99999999
UINT32 atousi8( char *str )
{
    UINT32 sum, pow;
    INT16 i;

    pow = 1;
    sum = 0;

    for (i = 7; i>=0; i--)
    {
        sum += ((str[i]-'0') * pow);
        pow *= 10;
    }

    return(sum);
}

BOOL IsHexChar( char myChar )
{   //Verify whether a character is a proper ASCII representation of a hex value
    BOOL retVal;
    if ( (myChar >= '0') || (myChar <= '9') )
    {
        retVal = TRUE;
    }
    else if ( (myChar >= 'a') || (myChar <= 'f') )
    {
        retVal = TRUE;
    }
    else if ( (myChar >= 'A') || (myChar <= 'F') )
    {
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }

    return retVal;
}

//Function: htousi2
//Inputs:
//  *str:   Pointer to 2-character hex string.  EX: "05" "F5" "5A"
//Outputs: none
//
//Description:
//Converts an unsigned hex number string to unsigned 8-bit int, range 0-0xFF
UINT8 htousi2( char *str )
{
    UINT8 i, v;
    UINT8 val = 0;
#define NDIG    2       //Number of digits

    for (i=0; i<NDIG; i++)
    {
        if ( (str[i]>='0') && (str[i]<='9') )
        {
            v = str[i] - '0';
        }
        else if ( (str[i]>='A') && (str[i]<='F') )
        {
            v = str[i] - 'A' + 0x0A;
        }
        else if ( (str[i]>='a') && (str[i]<='f') )
        {
            v = str[i] - 'a' + 0x0A;
        }
        else
        {
            v = 0;
        }
        val += (v << (4*(NDIG-i-1)));
    }

    return val;
}

//Function: cmp
//Inputs:   char *str1, str2:   Strings to compare
//          UINT8 len:          Length of both strings
//Outputs:  BOOL : TRUE means both strings match; FALSE means they don't match
//
//Description:
//Basic string compare function.  Checks if two strings match exactly.
BOOL cmp( char *str1, char *str2, UINT8 len )
{
    UINT8 i;
    for (i=0; i<len; i++)
    {
        if (str1[i] != str2[i])
        {
            break;
        }
    }
    if (i == len)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

//Function: isAlphaNumeric
//Inputs:   char byte       Byte to evaluate
//Outputs:  BOOL :          TRUE means byte is an alphanumeric character:
                            //0-9, a-z, or A-Z
BOOL isAlphaNumeric( char byte )
{
    BOOL isAlNu = FALSE;

    if ( (byte <= '9') && (byte >= '0') )
    {
        isAlNu = TRUE;
    }
    else if ( (byte <= 'Z') && (byte >= 'A') )
    {
        isAlNu = TRUE;
    }
    else if ( (byte <= 'z') && (byte >= 'A') )
    {
        isAlNu = TRUE;
    }

    return isAlNu;
}
