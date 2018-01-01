fn MagV vec = (sqrt((vec.x)^2 + (vec.y)^2 + (vec.z)^2))

fn GetLowIndex a = 
	(
		low = a[1]
		LIndex = 1
		for i = 1 to a.count do
			if a[i] < low do
				(
					low = a[i]
					LIndex = i
				)
		return LIndex
	)

fn sort2 a b = 
	(
		local Na = #(), Nb = #()
		local LowA = 999
		local LowI = -1
		for j = 1 to a.count do
			(
				for i = 1 to a.count do
					(
						if a[i] < LowA then
							(
								LowA = a[i]
								LowI = i
							) else (
								format "% is larger than %::%\n" a[i] LowA (a[i] > LowA)
							)
					)
				append Na a[LowI]
				append Nb b[LowI]
				a[LowI] = 999
			)
		return #(Na, Nb)
	)

fn crossP2 a b = a.x * b.y - a.y * b.x

fn Intersect r oMesh =
	(
		local nf = oMesh.numfaces
		local debug = false

		local rPoint, rNormal, rIndex, rUV
		
		-- first get t (where t > 0) for all faces in the object
		TArray = #()
		TIndices = #()
		for i = 1 to nf do
			(
				-- get the vertex indices of the current face
				CFace = getFace oMesh i
				-- get the vertex positions of the vace
				V0 = (getVert oMesh CFace.x)
				-- get the normal of the face
				N = getFaceNormal oMesh i
				d = -(dot V0 N)
				t = -((d + (dot N r.pos)) / (dot N r.dir))
				if t > 0.0 do
					(
						append TArray t
						append TIndices i
					)
			)

		if TArray.count != 0 do
			(
				-- i = 1 to TArray.count do 
				for i = 1 to TIndices.count do
					(
						iLowIndex = GetLowIndex TArray
						t = TArray[iLowIndex]
						N = getFaceNormal oMesh TIndices[iLowIndex]
						CFace = getFace oMesh TIndices[iLowIndex]
						V0 = (getVert oMesh CFace.x)
						V1 = (getVert oMesh CFace.y)
						V2 = (getVert oMesh CFace.z)
						CTVFace = getTVFace oMesh TIndices[iLowIndex]
						uv0 = (getTVert oMesh CTVFace.x)
						uv1 = (getTVert oMesh CTVFace.y)
						uv2 = (getTVert oMesh CTVFace.z)
						if (dot N r.dir) != 0 do
							(
								P0 = r.pos.x + r.dir.x * t
								P1 = r.pos.y + r.dir.y * t
								P2 = r.pos.z + r.dir.z * t
								P = [P0, P1, P2]
								P0 = r.pos.x + r.dir.x * t
								P1 = r.pos.y + r.dir.y * t
								P2 = r.pos.z + r.dir.z * t
								P = [P0, P1, P2]
								c1 = (crossP2 (V1 - V0) (P - V0))
								c2 = (crossP2 (V2 - V1) (P - V1))
								c3 = (crossP2 (V0 - V2) (P - V2))
								-- if c1, c2, and c3 are all positive, the point is inside the triangle
								if c1 >= 0 and c2 >= 0 and c3 >= 0 do	
									(
										-- Polygon Intersection!
										rPoint = P
										rNormal = N
										rIndex = iLowIndex
										-- calculate the new uv coordinate of the intersection point
										CTVFace = getTVFace oMesh rIndex
										TV0 = getTVert oMesh CTVFace.x
										TV1 = getTVert oMesh CTVFace.y
										TV2 = getTVert oMesh CTVFace.z
										UScale = (V1 - V0).x / P.x
										VScale = (V2 - V0).y / P.y
										format "P:%\n" P
										format "1:%\t2:%\t3:%\n" V0 V1 V2
										format "% %\n" UScale VScale
										return #(rPoint, rNormal, rIndex, rUV)
									)
							)
						TArray[iLowIndex] = 99999.9
					)
			)
	)