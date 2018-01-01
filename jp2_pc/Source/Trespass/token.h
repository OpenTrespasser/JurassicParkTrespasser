//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       gdidlgs.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    02-Jun-98   SHernd   Created
//
//---------------------------------------------------------------------------

#ifndef __TOKEN_H__
#define __TOKEN_H__

typedef enum tagLEVELFILETOKENTYPE
{
    LFTT_NULL,
    LFTT_WHITESPACE,
    LFTT_SEPERATOR,
    LFTT_VALUE,
    LFTT_HEXVALUE,
    LFTT_DOUBLEQUOTE,
    LFTT_ENDDOUBLEQUOTE,
    LFTT_SEMICOLON,
    LFTT_COMMA,
    LFTT_STRING,
    LFTT_NEGATIVE,
    LFTT_BACKGROUND,
    LFTT_SPRITE,
    LFTT_UICTRL,
    LFTT_END,
    LFTT_EOF
} LEVELFILETOKENTYPE, LFTT;

typedef struct tagKEYWORD
{
    LFTT    Token;
    char *  pszKeyword;
    DWORD   dwFlags;
} KEYWORD;

typedef struct tagWNDFILETOKEN
{
    LFTT        TokenType;
    int         iLen;

    union TOKENVALUES
    {
        char    ch;
        int     iVal;
        char    sz[255];
    } val;
} WNDFILETOKEN;

BOOL ReadWndFileToken(HANDLE hFile, WNDFILETOKEN * pwft);


#endif // __TOKEN_H__

