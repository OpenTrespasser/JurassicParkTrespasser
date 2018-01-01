/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CExpressionEvaluate and support functions
 *
 * Bugs:
 *
 * To do:
 *		Implement text prop expressions, RPN converter
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/ExpressionEvaluate.cpp                                   $
 * 
 * 10    9/03/98 9:31p Kmckis
 * Fixed assert output text
 * 
 * 9     8/31/98 1:59a Rwycko
 * More detailed assert about why a trigger hasn't loaded on by the 5th load pass
 * 
 * 8     8/01/98 10:06p Agrant
 * yet another fix for MSDEV code generation error!!!
 * 
 * 7     7/24/98 3:55p Agross
 * fixed expression evaluate bug
 * 
 * 6     7/08/98 9:25p Rwyatt
 * New errors for expression evaluateor in trigger base class
 * 
 * 5     6/23/98 1:55p Agrant
 * Workaround for MSDev code gen bug
 * 
 * 4     5/12/98 10:03p Rwyatt
 * Infix expression processing now handlkes the @. This means evaluate the trigger at fire
 * time, as a dynamic thing instead of waiting for it to fire.
 * 
 * 3     98/02/18 21:09 Speter
 * Changed #include SaveFile.hpp to shorter SaveBuffer.hpp.
 * 
 * 2     12/03/97 12:02p Agrant
 * Trigger load/save
 * 
 * 1     11/13/97 11:24p Rwyatt
 * Initial Implementation, Split off from the old trigger.cpp
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Trigger.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Loader/SaveBuffer.hpp"

//*********************************************************************************************
// RPN static class members of boolean expression evaluator.
volatile bool	CExpressionEvaluate::bExpStack[u4EXPRESSION_STACK_SIZE];		// fixed stack depth for now
uint32	CExpressionEvaluate::u4StackTop;


//*********************************************************************************************
// This will remove all white space characters from the source string as it copies it to the
// destination.
//
static __inline void RemoveWhiteSpace(char* str_dst, const char* str_src)
{
	while (*str_src)
	{
		if (*str_src>32)
		{
			*str_dst = *str_src;
			*str_dst++;
		}
		*str_src++;
	}
	*str_dst = 0x00;
}



//*********************************************************************************************
// This will check if the character at the pointer passed in is a operand symbol or a operator
//
static __inline bool bIsSymbol(char* pch)
{
	if (*pch==' ' || *pch==0x09 || *pch=='&' || *pch=='|' || 
		*pch=='^' || *pch=='='  || *pch=='(' || *pch==')' || 
		*pch=='!') 
	{
		return false;
	}

	return true;
}



//*********************************************************************************************
// This function checks if the character at the pointer is a VALID operator. This validation
// checks are only performed in debug mode, in release mode this function will return true if
// the character is an operator.
//
static __inline bool bIsOperator(char* pch)
{
	// white space always returns true
	if (*pch==' ' || *pch==0x09)
		return true;

	// binops have to check the next character to make sure it is another binop
	if (*pch=='&' || *pch=='|' || *pch=='^' || *pch=='=') 
	{
		Assert (!(*(pch+1)=='&' || *(pch+1)=='|' || *(pch+1)=='^' || 
					*(pch+1)=='=' || *(pch+1)==')' || *(pch+1)==0x00));
		return true;
	}

	// an open bracket must be followed by a symbol or a uniop or another bracket
	if  (*pch=='(')
	{
		Assert (!(*(pch+1)=='&' || *(pch+1)==')' || *(pch+1)=='|' || 
					*(pch+1)=='^' || *(pch+1)=='=' || *(pch+1)==0x00));

		return true;
	}

	// a close bracket cannot be followed by an uniop or open bracket
	if  (*pch==')')
	{
		Assert (!(*(pch+1)=='(' || *(pch+1)=='!'));

		return true;
	}

	// uniops cannot be followed by bi-ops or close brackets
	if  (*pch=='!')
	{
		Assert (!(*(pch+1)=='&' || *(pch+1)==')' || *(pch+1)=='|' || 
					*(pch+1)=='^' || *(pch+1)=='=' || *(pch+1)==0x00));
		return true;
	}

	return false;
}



//*********************************************************************************************
// This will validate all the symbols in the expression and true will only be returned if all
// the symbols are present. This is the only case in which the trigger can be constructed.
//
// In debug mode the form of the expression and its syntax is verified by Asserts.
//
bool CExpressionEvaluate::bValidateExpression
(
	CLoadWorld*				pload,		// The loader.
	const char*				str_expression
)
//*************************************
{
	char	str_current_symbol[128];
	char	str_condensed_exp[256];
	char*	str_exp = str_condensed_exp;
	char*	pstr;
	uint32	u4_form = 0;

	// make sure the string and the pointer are not
	Assert(str_expression);
	Assert(*str_expression);

	// we cannot find a binop to start with
	bool	b_binops = false;
	// we can have a uniop (included open brackets)
	bool	b_uniops= true;

	RemoveWhiteSpace(str_condensed_exp,str_expression);

	while (1)
	{
		pstr = str_current_symbol;

		while ( bIsOperator(str_exp) && *str_exp!=0x00)
		{
			// keep track of the expression form, at the end this should be zero
			if (*str_exp == '(')
				u4_form++;
			if (*str_exp == ')')
				u4_form--;

			// check for valid uniops
			if (*str_exp == '(' || *str_exp == '!')
			{
				Assert(b_uniops);
				b_binops = false;
			}

			// check for valid biops
			if (*str_exp == '&' || *str_exp == '|' || 
						*str_exp == '=' || *str_exp == '^' || *str_exp == ')')
			{
				Assert(b_binops);
				b_uniops = true;
			}

			str_exp++;
		}

		// have we hit the end of the expression string..
		if (*str_exp == 0x00)
			break;

		// until the next operator or space or end of string copy to local buffer
		while (bIsSymbol(str_exp) && *str_exp!=0x00)
		{
			*pstr = *str_exp;
			pstr++;
			str_exp++;
		}

		b_uniops = false;
		b_binops = true;

		*pstr = 0x00;

		// we now have the current symbol so lets search for it in the world database
		//dprintf("Expression symbol: %s\n", str_current_symbol);

		CInstance* pins;
		if (*str_current_symbol == '@')
		{
			// Remove any leading '@' before querying the world database for the symbol.

#if VER_TEST
			// check that the specified object exists in the GROFF file
			CGroffObjectName* pgon = pload->goiInfo.pgonFindObject( str_current_symbol+1 );

			if (pgon == 0)
			{
				char str_buffer[1024];
				sprintf(str_buffer, 
						"%s\n\nExpression symbol '%s' not present in groff file.\n", 
						__FILE__, 
						str_current_symbol+1);

				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
			}			
#endif
			// pins will be NULL is the symbol has not yet been loaded...
			pins = wWorld.ppartTriggerPartitionList()->pinsFindNamedInstance(str_current_symbol+1);

#if VER_TEST
			if (!pins && pload->iLoadPass >= 5)
			{
				char str_buffer[1024];
				sprintf(str_buffer, 
						"%s\n\nExpression symbol '%s' not present in load pass 5.\n", 
						__FILE__, 
						str_current_symbol+1);

				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
			}
#endif
		}
		else
		{
#if VER_TEST
			// check that the specified object exists in the GROFF file
			CGroffObjectName* pgon = pload->goiInfo.pgonFindObject( str_current_symbol );

			if (pgon == 0)
			{
				char str_buffer[1024];
				sprintf(str_buffer, 
						"%s\n\nExpression symbol '%s' not present in groff file.\n", 
						__FILE__, 
						str_current_symbol );

				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
			}			
#endif
			// pins will be NULL is the symbol has not yet been loaded...
			pins = wWorld.ppartTriggerPartitionList()->pinsFindNamedInstance(str_current_symbol);

#if VER_TEST
			if (!pins && pload->iLoadPass >= 5)
			{
				char str_buffer[1024];
				sprintf(str_buffer, 
						"%s\n\nExpression symbol '%s' not present in load pass 5.\n", 
						__FILE__, 
						str_current_symbol);

				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
			}
#endif

		}

		// if the symbol is not found return false and the loader will retry later..
		if (pins == NULL)
		{
			// But if the load pass is high, we won't ever find it, cuz it isn't there.  Warn the user.		
			return false;
		}

		CTrigger* ptr = ptCast<CTrigger>(pins);

		// the symbol was not a trigger name
		if (ptr == NULL)
		{

#if VER_TEST
			char str_buffer[1024];
			sprintf(str_buffer, 
					"%s\n\nExpression symbol '%s' does not reference a trigger.\n", 
					__FILE__,
					pins->strGetInstanceName() );

			if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
				DebuggerBreak();
#endif

			return false;
		}

		if (*str_exp == 0x00)
			break;
	}

#if VER_TEST
	if (u4_form != 0)
	{
		char str_buffer[1024];
		sprintf(str_buffer, 
				"%s\n\nTrigger expression has unmatched ().\n", 
				__FILE__);

		if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
			DebuggerBreak();
	}
#endif

	return true;
}


//*********************************************************************************************
// recursive function to convert infix to reverse polish, this function also adds the symbols
// and operators of the expression to the RPN evaluator.
//
char* CExpressionEvaluate::pchProcessRPN(char* str_exp)
{
	uint32	u4_uni_count[2];
	char	astr_symbol[2][128];
	uint32	u4_operator = 0;
	uint32	u4_symb = 0;

	// both symbols start of as null's
	astr_symbol[0][0] = 0x00;
	astr_symbol[1][0] = 0x00;
	u4_uni_count[0] = 0;
	u4_uni_count[1] = 0;

	while (1)
	{
		// after the current symbol emit the uni operators, we only have one so can hard code it!
		if (*str_exp == '!')
		{
			// keep a count of how may NOT operators there are...
			str_exp++;
			u4_uni_count[u4_symb]++;
		}

		// if an open bracket is encountered, re-enter the RPN evaluator with the sub expression
		if (*str_exp == '(')
		{
			str_exp++;
			str_exp = pchProcessRPN(str_exp);

			// after a sub expression has been converted its uniops need to be emmited.
			while (u4_uni_count[u4_symb])
			{
				AddExpressionOperator(BOOLEAN_NOT);
				u4_uni_count[u4_symb]--;

				//dprintf("! ");
			}
		}
		else
		{
			// make a local copy of the current symbol
			char* str_symbol = astr_symbol[u4_symb];

			// get the current symbol cahracter by character
			while (bIsSymbol(str_exp) && *str_exp!=0x00)
			{
				*str_symbol = *str_exp;
				*str_exp++;
				*str_symbol++;
			}
			*str_symbol = 0x00;				// terminate the symbol string
		}

		u4_symb++;

		Assert(u4_symb<=2);

		// if we have two symbols, the end of the string or the end of a sub expression
		// we need to emit some tokens..
		if (u4_symb == 2 || *str_exp==0x00 || *str_exp==')')
		{
			// time to output the current tokens.

			// output symbol 1, if there is one
			if (astr_symbol[0][0]!=0x00)
			{
				CInstance*	pins;
				bool		b_query = false;
				
				// If the first charcater of the symbol is an @ we need to add the following symbol as
				// a query symbol and not a static symbol.
				if (astr_symbol[0][0] == '@')
				{
					pins = wWorld.ppartTriggerPartitionList()->pinsFindNamedInstance(&astr_symbol[0][1]);
					b_query = true;
				}
				else
				{
					pins = wWorld.ppartTriggerPartitionList()->pinsFindNamedInstance(astr_symbol[0]);
					b_query = false;
				}

				Assert(pins);

				CTrigger* ptr	= ptCast<CTrigger>(pins);
				Assert(ptr);

				AddExpressionOperand(ptr, b_query);
				//dprintf("%s ",astr_symbol[0]);
			}

			// output any uniops along with symbol 1
			while (u4_uni_count[0])
			{
				AddExpressionOperator(BOOLEAN_NOT);
				u4_uni_count[0]--;

				//printf("! ");
			}

			// output symbol 2
			if (astr_symbol[1][0]!=0x00)
			{
				CInstance*	pins;
				bool		b_query = false;
				
				// If the first charcater of the symbol is an @ we need to add the following symbol as
				// a query symbol and not a static symbol.
				if (astr_symbol[1][0] == '@')
				{
					pins = wWorld.ppartTriggerPartitionList()->pinsFindNamedInstance(&astr_symbol[1][1]);
					b_query = true;
				}
				else
				{
					pins = wWorld.ppartTriggerPartitionList()->pinsFindNamedInstance(astr_symbol[1]);
					b_query = false;
				}

				Assert(pins);

				CTrigger* ptr	= ptCast<CTrigger>(pins);
				Assert(ptr);

				AddExpressionOperand(ptr, b_query);
				//dprintf("%s ",astr_symbol[1]);
			}

			while (u4_uni_count[1])
			{
				AddExpressionOperator(BOOLEAN_NOT);
				u4_uni_count[1]--;

				//printf("! ");
			}

			if (u4_operator)
			{
				// we have an operator, add the correct operator to the RPN expression.
				// Only binops are valid here, uniops should have already been processed

				//printf("%c ",(uint8)u4_operator);
				switch ((uint8)u4_operator)
				{
				case '&':
					AddExpressionOperator(BOOLEAN_AND);
					break;

				case '|':
					AddExpressionOperator(BOOLEAN_OR);
					break;

				case '^':
					AddExpressionOperator(BOOLEAN_XOR);
					break;

				case '=':
					AddExpressionOperator(BOOLEAN_EQUAL);
					break;

				default:
					// Invalid charatcer for operator...
					Assert(0);
					break;
				}
			}


			// if we are at the end of the string or the end of the expression we
			// return the next charatcer after the section. for the NULL case this
			// is OK even though the returned pointer is after the NULL. When the
			// end of string is met we must be at the top level of the evaluator
			// so the function will exit.
			if (*str_exp==')' || *str_exp==0x00)
			{
				return str_exp+1;
			}

			// We now have no symbols or operators, so are ready to start again..
			astr_symbol[0][0] = 0x00;
			astr_symbol[1][0] = 0x00;
			u4_operator = 0;
			u4_symb = 0;
		}
		else
		{
			// the next character must be a binary operator
			u4_operator = (uint32)*str_exp;
			str_exp++;
		}
	}
}



//*********************************************************************************************
// set the expression of this class to be the specified infix expression, the string is first
// converted to RPN
//
void CExpressionEvaluate::SetInfixExpression(const char* str_expression)
{
	char	str_exp[1024];

	// assert that the string fits into the local buffer
	Assert(strlen(str_expression)<1023);

	RemoveWhiteSpace(str_exp,str_expression);
	pchProcessRPN(str_exp);
}


//*********************************************************************************************
//
bool CExpressionEvaluate::EvaluateExpression()
{
	// no expression so must be TRUE
	if (u4ExpCount==0)
		return true;

	u4StackTop = 0;

	for (uint32 u4_i=0;u4_i<u4ExpCount;u4_i++)
	{
		if (aelExpression[u4_i].u4State & EELEMENT_STATE_OPERATOR)
		{
			switch ((uint32)aelExpression[u4_i].ptrTrigger)
			{
			// these use bitwise operations because C does not have a logical exclusive or, I
			// supose it would be identical to a bitwise exclusive or.
			case (uint32)BOOLEAN_AND:
			{
				bool b_temp1 = Pop();
				bool b_temp2 = Pop();
				Push(b_temp1 & b_temp2);
				break;
			}
			case (uint32)BOOLEAN_OR:
			{
				bool b_temp1 = Pop();
				bool b_temp2 = Pop();
				Push(b_temp1 | b_temp2);
				break;
			}
			case (uint32)BOOLEAN_XOR:
			{
				bool b_temp1 = Pop();
				bool b_temp2 = Pop();
				Push(b_temp1 ^ b_temp2);
				break;
			}
			case (uint32)BOOLEAN_NOT:
				Push(!Pop());
				break;

			case (uint32)BOOLEAN_EQUAL:
				Push(Pop()==Pop());
				break;

			default:
				// Invalid operator
				Assert(0);
			}
		}
		else
		{
			// Push either the dynamic or static state of the current trigger onto the stack.
			if (aelExpression[u4_i].u4State & EELEMENT_STATE_QUERY)
			{
				// Push the dynamic state of the trigger onto the evalaution stack
				Push( aelExpression[u4_i].ptrTrigger->bEvaluateNow() );
			}
			else
			{
				// push the a true or false value on the stack for the given element
				Push((aelExpression[u4_i].u4State & EELEMENT_STATE_FIRED)?true:false);
			}
		}
	}

	// return the top of the stack.....
	return Result();
}


//*****************************************************************************************
char * CExpressionEvaluate::pcSave(char *  pc) const
{
	// Save the state portion of the expression elements.
	for (int i = 0; i < u4EXPRESSION_STACK_SIZE; ++i)
	{
		pc = pcSaveT(pc, aelExpression[i].u4State);
	}

	return pc;
}


//*****************************************************************************************
const char * CExpressionEvaluate::pcLoad(const char *  pc)
{
	// Load the state portion of the expression elements.
	for (int i = 0; i < u4EXPRESSION_STACK_SIZE; ++i)
	{
		pc = pcLoadT(pc, &aelExpression[i].u4State);
	}

	return pc;
}



