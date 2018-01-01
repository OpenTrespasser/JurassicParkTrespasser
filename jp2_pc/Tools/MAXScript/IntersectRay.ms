
fn MaxComp N = 
	(
		m = (abs N.x)
		i = 1
		if (abs N.y) > m do
			(
				m = (abs N.y)
				i = 2
			)
		if (abs N.z) > m do
			(
				m = (abs N.z)
				i = 3
			)
		return i
	)

fn Intersect r oMesh =
	(
		local nf = oMesh.numfaces
		local debug = false

		local rPoint, rNormal, rIndex, rUV
		
		-- first get t (where t > 0) for all faces in the object
		TmpArray = #()
		TmpIndices = #()
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
						append TmpArray t
						append TmpIndices i
					)
			)

		if TmpArray.count != 0 do
			(
				aTemp = (SortUp2 TmpArray TmpIndices)
				TArray = aTemp[1]
				TIndices = aTemp[2]
				
				for i = 1 to TIndices.count do
					(
						t = TArray[i]
						if debug do format "\n\nFace:%\n" TIndices[i]
						-- get the vertex indices of the current face
						CFace = getFace oMesh TIndices[i]
						-- get the vertex positions of the vace
						V0 = (getVert oMesh CFace.x)
						V0 = #(V0.x, V0.y, V0.z)
						V1 = (getVert oMesh CFace.y)
						V1 = #(V1.x, V1.y, V1.z)
						V2 = (getVert oMesh CFace.z)
						V2 = #(V2.x, V2.y, V2.z)
						local V = #(V0, V1, V2)
						-- First Rejection test (t>0)
						if t > 0.0 then
							(
								N = getFaceNormal oMesh TIndices[i]
								-- second rejection test, are the normals parallel?
								if (dot N r.dir) != 0 do
									(
										inter = false
										P0 = r.pos.x + r.dir.x * t
										P1 = r.pos.y + r.dir.y * t
										P2 = r.pos.z + r.dir.z * t
										P = #(P0, P1, P2)
										if debug do
											(
												format "Intersection Point = %\n" [p0,p1,p2]
												s = sphere()
												s.radius = .25
												s.pos = [p0,p1,p2]
											)
										-- determine the max component of N
										i0 = MaxComp N
										if debug do format "Maximum component:%\n" i0
										if i0 == 1 do (i1 = 2;i2 = 3)
										if i0 == 2 do (i1 = 3;i2 = 1)
										if i0 == 3 do (i1 = 1;i2 = 2)
										u0 = P[i1] - V[1][i1]
										v0 = P[i2] - V[1][i2]
										u1 = V[2][i1] - V[1][i1]
										u2 = V[3][i1] - V[1][i1]
										v1 = V[2][i2] - V[1][i2]
										v2 = V[3][i2] - V[1][i2]
		
										if debug do format "u0:%\t\tu1:%\t\tu2:%\nv0:%\t\tv1:%\t\tv2:%\n" u0 u1 u2 v0 v1 v2
										if u1 == 0 then
											(
												beta = u0/u2 as float
												if debug do format "beta:%\n" beta
												if (beta >= 0.0) and (beta <= 1.0) do
													(
														alpha = (v0 - beta * v2) / v1
														if debug do format "alpha:%\n" alpha
														inter = ((alpha >= 0) and ((alpha + beta) <=1))
													)
											) else (
												beta = ((v0 * u1) - (u0 * v1)) / ((v2 * u1) - (u2 * v1))
												if debug do format "Beta:%\n" beta
												if ((beta >= 0.0) and (beta <= 1.0)) do
													(
														alpha = (u0 - (beta * u2)) / u1
														if debug do format "Alpha:%\n" alpha
														if (inter = ((alpha >= 0) and ((alpha + beta) <=1))) do
															(
																CTVFace = getTVFace oMesh TIndices[i]
																-- get the TVerts of the face
																tv0 = (getTVert oMesh CTVFace.x)
																tv1 = (getTVert oMesh CTVFace.y)
																tv2 = (getTVert oMesh CTVFace.z)
																local tv = #(tv0, tv1, tv2)
																rUV = ((tv0 * (1 - (alpha + beta))) + (tv1 * alpha) + (tv2 * beta))
																rPoint = P
																-- get the normal of the face
																rN = N
																rIndex = TIndices[i]
																return #(rPoint, rN, rIndex, rUV)
															)
													)
											)
									)
							)
					)
			)
		return undefined
	)

-- returns the magnitude of a vector
fn MagV vec = (sqrt((vec.x)^2 + (vec.y)^2 + (vec.z)^2))
-- returns a normalized vector between two objects
fn GetNormalDirVec o1 o2 = normalize (o2.pos - o1.pos)
-- returns a non-normalized vector between two objects
fn GetDirVec o1 o2 = (o2.pos - o1.pos)
-- returns a ray from one object to another
fn CreateRay o1 o2 = 
	return (ray o1.pos (GetNormalDirVec o2 o1))
