
template<class I> class CSet
{
protected:
	uint32	u4Bits;						// Contains the bitset.

public:

	class CSetHelperConst
	{
	protected:
		const uint32&	ru4Var;			//lint !e1725
										// Reference points to a bitset variable.
		
		uint32			u4Mask;			// Mask indicates which bits to read or write.

	public:
		//**************************************************************************************
		//
		// Constructor.
		//

		// Straight member specification.  Sets up which things to operate on.
		CSetHelperConst(const uint32& ru4_var, uint32 u4_mask)
			: ru4Var(ru4_var), u4Mask(u4_mask) {}

	};
};
