//
// Compiler is confused about the existence of an overloaded operator function when passing
// parameters by reference.
//

template<class TR = float> class A0
{
};

template<class TR = float> class B0
{
public:
	friend inline A0<TR> operator *(const A0<TR>& a, const B0<TR>& b)
	{
		return a;
	}
};


A0<> func_a(const A0<>& a, const B0<>& b)
{
	// [+5.3] Generates error:
	//
	// error C2676: binary '*' : 'const class A0<float>' does not define this operator or a conversion to a type acceptable to the predefined operator (new behavior; please see help)
	//
	// Note that 'b' is passed by reference.
	return a * b;
}

A0<> func_b(const A0<>& a, B0<> b)
{
	// But this is fine!
	//
	// Note that 'b' is passed by value.
	return a * b;
}



//
// Same construct, but this time the overloaded operator is defined as a global function,
// instead of as a (global) friend function inside the class body.
//

template<class TR = float> class A1
{
};

template<class TR = float> class B1
{
};


template<class TR> inline A1<TR> operator *(const A1<TR>& a, const B1<TR>& b)
{
	return a;
}

A1<> func_c(const A1<>& a, const B1<>& b)
{
	// This is fine....
	return a * b;
}
