// 
// Compiler won't do certain conversions when matching non-template arguments in template 
// functions.
//

//
// It won't convert class C to const class C& !  But will convert int to const int& !
//

	class CC {
	public:
		CC(int);
	};

	template<class X> void func1(const X&, const int&);
	template<class X> void func2(const X&, const CC&);

	void test1a()
	{
		// This works.  It converts the second parameter from int to const int&.
		func1(4, 3);
		// error C2664: 'func2' : cannot convert parameter 2 from 'class CC' to 'const class CC &' 
		func2(4, CC(3));
	}

// 
// It won't convert a derived reference to a base reference.
//

	class DD: public CC {
	public:
		DD(int);
	};

	void test1b(const DD& dd)
	{
		// error C2664: 'func2' : cannot convert parameter 2 from 'const class DD' to 'const class CC &' 
		func2(4, dd);
	}
