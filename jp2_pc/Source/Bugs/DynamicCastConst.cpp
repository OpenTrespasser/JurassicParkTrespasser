// Can't perform dynamic_cast on const pointers.

class A
{
public:
	virtual ~A();
};

class B: public A
{
};

void test()
{
	A* pa = new B;
	B *pb = dynamic_cast<B*>(pa);

	// error C2682: cannot use dynamic_cast to convert from 'const class A *' to 'const class B *'
	const A* cpa = new B;
	const B *cpb = dynamic_cast<const B*>(cpa);
}


