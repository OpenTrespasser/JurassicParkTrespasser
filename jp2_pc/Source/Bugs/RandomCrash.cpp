//
// Some more or less random, hacked code, that makes the compiler crash (!).
//

typedef float	TReal;

template<class TR = TReal> class CVector2
{
public:
	TR		tX, tY;

	CVector2(TR t_x, TR t_y)
	{
	}

	friend inline CVector2<TR> operator *(const CVector2<TR>& v2, TR t)
	{
		return CVector2<TR>(v2.tX * t, v2.tY * t);
	}

	friend inline TR operator *(const CVector2<TR>& v2_a, const CVector2<TR>& v2_b)
	{
		return v2_a.tX * v2_b.tX  +  v2_a.tY * v2_b.tY;
	}
};


template<class TR = TReal> class CVector3
{
public:
	TR		tX, tY, tZ;


	CVector3(TR t_x, TR t_y, TR t_z)
	{
	}


	CVector3(const CVector2<TR>& v2)
		: tX(v2.tX), tY(v2.tY)
	{
	}


	friend inline CVector3<TR> operator *(const CVector3<TR>& v3, TR t)
	{
		return CVector3<TR>(v3.tX * t, v3.tY * t, v3.tZ * t);
	}

	friend inline TR operator *(const CVector3<TR>& v3_a, const CVector3<TR>& v3_b)
	{
		return v3_a.tX * v3_b.tX  +  v3_a.tY * v3_b.tY  +  v3_a.tZ * v3_b.tZ;
	}


};


template<class TR = TReal> class CMatrix3
{
};

	template<class TR> inline CMatrix3<TR> operator *(const CMatrix3<TR>& mx3_a, const CMatrix3<TR>& mx3_b)
	{
	}


	template<class TR> CVector3<TR> operator *(const CVector3<TR>& v3, const CMatrix3<TR>& mx3) 
	{
	}



template<class TR = TReal> class CRotate3
{
public:

	friend CRotate3<TR> operator *(const CRotate3<TR>& r3_a, const CRotate3<TR>& r3_b);

	friend CVector3<TR> operator *(const CVector3<TR>& v3, const CRotate3<TR>& r3);

};

template<class TR = TReal> class CPlacement3
{
public:
	CRotate3<TR>	r3T;

	friend inline CVector3<TR> operator *(const CVector3<TR>& v3, const CPlacement3<TR>& p3)
	{
	}
};





class CPlane
{
public:
	CVector3<>	d3Normal;
};


class CBoundVolSphere
{
public:
	CVector3<> v3Pos;

	CBoundVolSphere operator *(const CPlacement3<>& p3)
	{
		return CBoundVolSphere(p3 * v3Pos, rRadius);
	}
};
