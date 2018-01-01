//
// Nested types used in template classes.
//

template<class T> class TClass
{
public:
	T*	at;

	TClass()
	{
		at = new T[10];
	}

	TClass(int count)
		// An error occurs here when using T in the initialiser list.
		// error C2321: syntax error : unexpected 'Outer::Inner'
		: at(new T[count])
	{
	}
};

struct Outer
{
	int		X;

	struct Inner
	{
		int		Y;
	};

	TClass<Inner> in;
};


