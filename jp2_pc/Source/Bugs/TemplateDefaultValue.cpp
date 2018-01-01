//
// Template classes with value templates with default values do not work.
//

	template<float F = 4.2f> class Bummy
	{
	};

	class Crummy: public Bummy<>
	{
		Crummy()
			: Bummy<>()
// 5.0: error C2614: 'Crummy' : illegal member initialization: 'Bummy<1.e66>' is not a base or member
		{
		}

		void Funky(Bummy<>& bummy);

	};

	void Crummy::Funky(Bummy<>& bummy)
// 5.0: error C2511: 'Funky' : overloaded member function not found in 'Crummy'
	{
	}