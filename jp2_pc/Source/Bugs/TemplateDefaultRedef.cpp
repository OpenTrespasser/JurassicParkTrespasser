//
// Bogus redefinition error when using default template parameters.
//

	// This works as it should:
	template<class T> class Gummy
	{
	};

	// No error when defining T.
	typedef int	T;

	// However, when given a default type, it breaks.
	template<class U = int> class Mummy
	{
	};

	// error C2371: 'U' : redefinition; different basic types
	typedef int U;


