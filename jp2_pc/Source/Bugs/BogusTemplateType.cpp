// Doesn't catch invalid template type.

template<class T> class X
{
public:
	// No error! Bogus isn't defined, but compiler issues no error.
	X<Bogus>& operator =(const X<T>&);
};
