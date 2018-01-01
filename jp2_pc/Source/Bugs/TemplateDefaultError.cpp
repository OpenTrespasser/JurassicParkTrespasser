//
// If a template class with default template type is not followed
// by another template class without default template type, spurious syntax errors occur.
//

	template<class V = int> class Bummy
	{
	};

	// If you uncomment the following, the error disappears.
/*
	template<class W> class Yummy
	{
	};
*/

	class CC 
	{
	public:
		// error C2059: syntax error : ')'
		CC();
	};

