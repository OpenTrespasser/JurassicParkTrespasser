//
// Bogus ambiguity in inheritance with templates.
//

	template<class T> class A
	{
	public:
		int func(int x);
	};

	// Declare some global functions which operate on A<T>.
	template<class T> int gunk(A<T>& a, int x);

	template<class T> int hunk(int x, A<T>& a);


	// Now inherit template class B from template class A.
	template<class T> class B: public A<T>
	{
	public:
		int func(int x);
	};

	// Overload the global functions to work with B<T> (supposedly).
	template<class T> int gunk(B<T>& b, int x);

	template<class T> int hunk(int x, B<T>& b);


	void test2()
	{
		B<float> b;

		// Calling member functions resolves correctly.
		b.func(1);

		// error C2668: 'gunk': ambiguous call to overloaded function
		gunk(b, 2);
		// This is clearly wrong.  This call exactly matches gunk(B<T>&, int) and not gunk(A<T>&, int).


		// error C2668: 'hunk': ambiguous call to overloaded function
		hunk(3, b);
		// This happens regardless of parameter order.

		// Member function also works correctly when converting parameter type.
		b.func(1.0);

		// error C2665: 'gunk': none of the 2 overloads can convert parameter 2 from type 'const double'
		gunk(b, 2.0);
		// Now it says there are *no* matches, because it won't convert double to int.

		// error C2668: 'hunk': ambiguous call to overloaded function
		hunk(3.0, b);
		// Now it's back to saying there are ambiguous matches.  Jeez.
	}

