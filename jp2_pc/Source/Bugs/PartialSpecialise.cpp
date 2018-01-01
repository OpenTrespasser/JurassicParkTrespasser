//
// Partially specialising a multiple template doesn't work.
//

	// Template function with 2 template parameters.
	template<class T, class U> inline int ugly2(T t, U u)
	{
		return t + u;
	}

	// Specialise just the U class.
	template<class T> inline int ugly2(T t, char u)
	{
		return t + 2*u;
	}

	void test_this2()
	{
		// This should match ugly2(?, char), and not ugly2(?, ?).
		// 5.3: error C2667: 'ugly2' : none of 2 overload have a best conversion.
		// 4.2 and 5.3: error C2668: 'ugly2' : ambiguous call to overloaded function
		ugly2(3, 'a');
	}


