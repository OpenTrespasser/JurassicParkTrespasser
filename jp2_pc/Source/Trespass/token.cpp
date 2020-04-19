//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       token.cpp
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



#include "precomp.h"
#pragma hdrstop

#include "token.h"


BOOL                g_bIsPrevRead = FALSE;
BYTE                g_bPrevRead;

#define NUM_TOKENS  6
KEYWORD    g_Tokens[] =
{
    { LFTT_NULL,            "",             0},
    { LFTT_SEMICOLON,       ";",            0},
    { LFTT_COMMA,           ",",            0},
    { LFTT_BACKGROUND,      "BACKGROUND",   0},
    { LFTT_UICTRL,          "UICTRL",       0},
    { LFTT_SPRITE,          "SPRITE",       0},
    { LFTT_END,             "END",          0},
};


BOOL IsTerminator(char ch)
{
    return (strchr("\"\r\n, ;\t-", ch) != NULL);
}

BOOL IsSeperator(char ch)
{
    return (strchr("\"- ;", ch) != NULL);
}

BOOL IsWhitespace(char ch)
{
    return (strchr("\r\n \t", ch) != NULL);
}

BOOL IsHex(char ch)
{
    return (strchr("01234567890aAbBcCdDeEfF", ch) != NULL);
}

BOOL IsNumber(char ch)
{
    return (strchr("01234567890", ch) != NULL);
}



BOOL ReadWndFileToken(HANDLE hFile, WNDFILETOKEN * pwft)
{
    BOOL    bRet;
    DWORD   dwRead;
    BYTE    bRead;
    BOOL    bFinished;
    int     iRead;
    BOOL    bDeterminedType;
    BOOL    bNegative;
    BOOL    bAlwaysAccept;
    BOOL    bHex;
    BOOL    bErr = FALSE;
    int     i;

    Assert(hFile != INVALID_HANDLE_VALUE);

    // initialize the Token
    memset(pwft, 0, sizeof(WNDFILETOKEN));

    bFinished = FALSE;
    iRead = 0;
    bDeterminedType = FALSE;

    //
    // Prime the token reader by reading 1 character and giving a whirl on where
    // to go from there.
    //
    if (!g_bIsPrevRead)
    {
        bErr = ReadFile(hFile, &bRead, sizeof(bRead), &dwRead, NULL);
        if (!bErr)
        {
            TraceError(("ReadWndFileToken() -- Error reading File"));
            goto Error;
        }
    }
    else
    {
        g_bIsPrevRead = FALSE;
        dwRead = sizeof(bRead);
        bRet = TRUE;
        bRead = g_bPrevRead;
    }

    // if the read was successful and we didn't read a whole byte then
    // the end-of-file has been reached
    if (bErr && (sizeof(bRead) != dwRead))
    {
        pwft->TokenType = LFTT_EOF;
        pwft->val.sz[0] = 0;
        bFinished = TRUE;
    }
    else
    {
        while ((IsWhitespace(bRead) || (bRead == ';')) && !bFinished)
        {
            while (IsWhitespace(bRead) && !bFinished)
            {
                // remove as much whitespace as possible before the valid
                // characters
                bErr = ReadFile(hFile, &bRead, sizeof(bRead), &dwRead, NULL);
                if (!bErr)
                {
                    TraceError(("ReadWndFileToken() -- Error reading File"));
                    goto Error;
                }

                // if the read was successful and we didn't read a whole byte then
                // the end-of-file has been reached
                if (bErr && (sizeof(bRead) != dwRead))
                {
                    pwft->TokenType = LFTT_EOF;
                    pwft->val.sz[0] = 0;
                    bFinished = TRUE;
                }
            }

            // a comment has been read in so skip til the end of the line
            if (bRead == ';')
            {
                while (bRead != '\r' && bRead != '\n' && !bFinished)
                {
                    bErr = ReadFile(hFile, &bRead, sizeof(bRead), &dwRead, NULL);
                    if (!bErr)
                    {
                        TraceError(("ReadWndFileToken() -- Error reading File"));
                        goto Error;
                    }

                    // if the read was successful and we didn't read a whole byte then
                    // the end-of-file has been reached
                    if (bErr && (sizeof(bRead) != dwRead))
                    {
                        pwft->TokenType = LFTT_EOF;
                        pwft->val.sz[0] = 0;
                        bFinished = TRUE;
                    }
                }
            }
        }

        pwft->val.sz[iRead++] = bRead;
    }

    if (!bFinished)
    {
        //
        // Now that we have a valid first character, try and determine the
        // remaining part of the token
        //
        bNegative = FALSE;
        if (pwft->val.sz[0] == '-')
        {
            bNegative = TRUE;
        }

        if (pwft->val.sz[0] == '\"')
        {
            iRead = 0;
            bAlwaysAccept = FALSE;
            while (!bFinished)
            {
                bErr = ReadFile(hFile, &bRead, sizeof(bRead), &dwRead, NULL);

                // If we reach the end-of-file marker while reading a string
                // certainly the file is in error
                if ((!bErr) || (sizeof(bRead) != dwRead))
                {
                    TraceError(("ReadWndFileToken() -- Error reading File"));
                    goto Error;
                }

                if ((bRead == '\"') && !bAlwaysAccept)
                {
                    bFinished = TRUE;
                }
                else if (bRead == '\\' && !bAlwaysAccept)
                {
                    bAlwaysAccept = TRUE;
                    // accept the next character no matter what. could be used
                    // for enclosed "s.  We also just ignore this read in char
                }
                else
                {
                    pwft->val.sz[iRead++] = bRead;
                    bAlwaysAccept = FALSE;
                }
            }

            pwft->TokenType = LFTT_STRING;
            pwft->iLen = iRead;
            pwft->val.sz[iRead] = '\0';
        }
        else
        {
            // at this point we have a 2 major possibilities.  We are either going
            // to examine a number or a true token.
            bHex = FALSE;

            if (IsNumber(bRead) || (bRead == '-'))
            {
                pwft->val.iVal = 0;

                while (!bFinished)
                {
                    if ((bRead == 'x' || bRead == 'X'))
                    {
                        iRead++;
                        bHex = TRUE;
                    }
                    else if (IsNumber(bRead) || IsHex(bRead))
                    {
                        iRead++;
                        if (bHex)
                        {
                            pwft->val.iVal *= 16;
                            if (bRead > 'A')
                            {
                                pwft->val.iVal += toupper(bRead) - 55;
                            }
                            else
                            {
                                pwft->val.iVal += (bRead - '0');
                            }
                        }
                        else
                        {
                            pwft->val.iVal = (pwft->val.iVal * 10) + (bRead - '0');
                        }
                    }
                    else if ((bRead == '-') && (iRead == 1))
                    {
                        ;
                    }
                    else
                    {
                        g_bIsPrevRead = TRUE;
                        g_bPrevRead = bRead;
                        bFinished = TRUE;
                    }

                    if (!bFinished)
                    {
                        bErr = ReadFile(hFile, &bRead, sizeof(bRead), &dwRead, NULL);
                        if (!bErr)
                        {
                            TraceError(("ReadWndFileToken() -- Error reading File"));
                            goto Error;
                        }

                        // if the read was successful and we didn't read a whole byte then
                        // the end-of-file has been reached
                        if (bErr && (sizeof(bRead) != dwRead))
                        {
                            pwft->TokenType = LFTT_EOF;
                            bFinished = TRUE;
                        }
                    }
                }

                if (bNegative)
                {
                    pwft->val.iVal *= -1;
                }

                pwft->TokenType = LFTT_VALUE;
            }
            else
            {
                while (!IsTerminator(bRead) && !bFinished)
                {
                    // Read valid characters until a termination character is read
                    bErr = ReadFile(hFile, &bRead, sizeof(bRead), &dwRead, NULL);
                    if (!bErr)
                    {
                        TraceError(("ReadWndFileToken() -- Error reading File"));
                        goto Error;
                    }

                    // if the read was successful and we didn't read a whole byte then
                    // the end-of-file has been reached
                    if (bErr && (sizeof(bRead) != dwRead))
                    {
                        pwft->TokenType = LFTT_EOF;
                        pwft->val.sz[0] = 0;
                        bFinished = TRUE;
                    }
                    else if (!IsTerminator(bRead))
                    {
                        pwft->val.sz[iRead++] = bRead;
                    }
                }

                if (IsSeperator(bRead))
                {
                    g_bIsPrevRead = TRUE;
                    g_bPrevRead = bRead;
                }

                // Determine token type or NAME
                for (i = 0; i < NUM_TOKENS; i++)
                {
                    if (strcmp(g_Tokens[i].pszKeyword, pwft->val.sz) == 0)
                    {
                        pwft->TokenType = g_Tokens[i].Token;
                        break;
                    }
                }
            }
        }
    }

    bRet = TRUE;
    pwft->iLen = iRead;

Cleanup:
    return bRet;

Error:
    TraceError(("ReadWndFileToken()"));
    bRet = FALSE;
    goto Cleanup;
}


