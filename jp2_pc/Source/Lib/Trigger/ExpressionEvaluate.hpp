/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of CObjectTrigger defined in Trigger.hpp
 *
 * Contents:
 *		CExpressionEvaluate
 *
 * Bugs:
 *
 * To do:
 *		Implement text prop expressions
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/ExpressionEvaluate.hpp                                   $
 * 
 * 7     7/30/98 2:39a Kmckis
 * changed u4EXPRESSION_STACK_SIZE = 16 for some longer expressions.
 * 
 * 6     7/08/98 9:25p Rwyatt
 * New errors for expression evaluateor in trigger base class
 * 
 * 5     7/04/98 8:19p Rwyatt
 * change UpdateSymbolState to bUpdateSymbolState. The function now returns true or false
 * depening on if it updated the state of the expression
 * 
 * 4     6/23/98 1:55p Agrant
 * Workaround for MSDev code gen bug
 * 
 * 3     5/12/98 10:03p Rwyatt
 * Changed expression evaluate code to handle the dynamic query of triggers
 * 
 * 2     12/03/97 12:02p Agrant
 * Trigger load/save
 * 
 * 1     11/13/97 11:34p Rwyatt
 * Initial Implementation of the expression evaluator which can now be attached to any trigger
 * type. There are also fnctions to convert from infix to rpn
 * 
 **********************************************************************************************/


#ifndef HEADER_LIB_TRIGGER_EXPRESSION_HPP
#define HEADER_LIB_TRIGGER_EXPRESSION_HPP


// maximum size of the reverse polish evaluation stack
#define u4EXPRESSION_STACK_SIZE	16

class CTrigger;

//*********************************************************************************************
// These operators are stored in place of a trigger handle. they do not have to be registered
// with the Loader because a flag present in the expression states that this element is an
// operator..
#define BOOLEAN_AND		(0x0)
#define BOOLEAN_OR		(0x1)
#define BOOLEAN_NOT		(0x2)
#define BOOLEAN_XOR		(0x3)
#define BOOLEAN_EQUAL	(0x4)


//*********************************************************************************************
// prefix: ee
//
#define EELEMENT_STATE_NOT_FIRED	0
#define EELEMENT_STATE_FIRED		1
#define EELEMENT_STATE_OPERATOR		2
#define EELEMENT_STATE_QUERY		4


struct SExpressionElement
{
	CTrigger*		ptrTrigger;
	uint32			u4State;
};


//*********************************************************************************************
//
class CExpressionEvaluate
// Prefix: ee
// A boolean expression evaluator, the expression is based on handles which are contained in the
// structure above. By default all operands are false
// The expression is stored in reverse polish in a list.
//**************************************
{
public:
	SExpressionElement	aelExpression[u4EXPRESSION_STACK_SIZE];
	uint32				u4ExpCount;
	volatile static bool			bExpStack[u4EXPRESSION_STACK_SIZE];		// fixed stack depth for now
	static uint32		u4StackTop;


	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CExpressionEvaluate()
	{
		u4ExpCount=0;
	}


	//******************************************************************************************
	// remove the existing expression so another can be put in its place
	void RemoveExpression()
	{
		u4ExpCount=0;
	}


	//******************************************************************************************
	//
	void SetInfixExpression(const char* str_expression);


	//******************************************************************************************
	//
	bool EvaluateExpression();


	//******************************************************************************************
	//
	// elements should be added in the reverse polish order. This was going to be a protected
	// function and elements would have been added through a compile function so strings could
	// have been passed in. This will be done by the groff compiler so this is the only 
	// function that is required.....
	//
	// Normal Exptression are evaluated on the fired state of triggers, some expressions do
	// not want to know if the trigger has fired but if it is currently firing. Set the
	// bool to true if you want this behaviour
	//
	void AddExpressionOperand(CTrigger* ptr_trig, bool b_query = false)
	{
		Assert(u4ExpCount<u4EXPRESSION_STACK_SIZE);

		aelExpression[u4ExpCount].ptrTrigger = ptr_trig;
	
		// Should we listen for this trigger being fired or should  we ask what its current
		// state is.
		if (b_query)
		{
			aelExpression[u4ExpCount].u4State = EELEMENT_STATE_QUERY;
		}
		else
		{
			aelExpression[u4ExpCount].u4State = EELEMENT_STATE_NOT_FIRED;
		}
		u4ExpCount++;
	}


	//******************************************************************************************
	void AddExpressionOperator(uint32 u4_operator)
	{
		Assert(u4ExpCount<u4EXPRESSION_STACK_SIZE);

		aelExpression[u4ExpCount].ptrTrigger=(CTrigger*)u4_operator;
		aelExpression[u4ExpCount].u4State = EELEMENT_STATE_OPERATOR;
		u4ExpCount++;
	}


	//*****************************************************************************************
	// walk the expression list of this trigger and update any references
	//
	bool bUpdateSymbolState(CTrigger* ptr_trigger)
	{
		bool b_res = false;

		for (uint32 u4_i=0;u4_i<u4ExpCount;u4_i++)
		{
			// if this element is not an operator and matched the handle passed in set its state
			// to fired and return true
			if ( (aelExpression[u4_i].ptrTrigger==ptr_trigger) && 
				 ((aelExpression[u4_i].u4State & EELEMENT_STATE_OPERATOR) == 0) )
			{
				aelExpression[u4_i].u4State |= EELEMENT_STATE_FIRED;
				b_res = true;
			}
		}

		return b_res;
	}


	//******************************************************************************************
	//
	bool Result()
	{
		Assert(u4StackTop==1);
		return Pop();
	}


	//******************************************************************************************
	//
	void Push(bool b_state)
	{
		Assert(u4StackTop<u4EXPRESSION_STACK_SIZE);

		bExpStack[u4StackTop]=b_state;
		u4StackTop++;
	}


	//******************************************************************************************
	//
	bool Pop()
	{
		u4StackTop--;
		Assert(u4StackTop>=0)
		return bExpStack[u4StackTop];
	}

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

	//******************************************************************************************
	//
	static bool bValidateExpression(CLoadWorld*	p_load, const char* str_expression);


private:
	//******************************************************************************************
	//
	char* pchProcessRPN(char* str_expression);
};


#endif