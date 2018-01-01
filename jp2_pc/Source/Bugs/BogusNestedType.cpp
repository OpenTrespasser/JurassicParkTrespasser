// Compiler cannot differentiate between nested types of the same name in different classes,
// allowing erroneous code to compile.

class A
{
public:
	struct X
	{
		int a, b;
	};

	// A takes a parameter of type A::X.
	A(X x);
};

class B
{
public:
	struct X
	{
		char* str;
	};

	B()
	{
		X x = {"Bug"};

		// No error! We pass a parameter of type B::X, but A() takes it without error or warning.
		A a(x);
	}
};