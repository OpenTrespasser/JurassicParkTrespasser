
	template<class T = int> class X
	{
	public:
		void member(T t) {}
		friend void global(X<T> x, T t) {}
		friend void global2(X<T> x, T t);
	};

	template<class T = int> void global2(X<T> x, T t);

	// Allowed to specialise a member function.
	void X<int>::member(int t) {}

	// error C2084: function 'void __cdecl gunc(class X<int>,int)' already has a body
	void global(X<int> x, int t) {}
	// But can't specialise a friend declared in-class.

	// Allowed to specialise a global friend function.
	void global2(X<int> x, int t) {}

