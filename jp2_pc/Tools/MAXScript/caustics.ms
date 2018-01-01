-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- * 					implementation of caustics.ms
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- * 				Caustics is a simple script raytracer that generates bitmaps from a water object,
-- *				a terrain object and a light
-- *
-- **********************************************************************************************
fileIn "SortFns.ms"
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
		fi = false
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
														if ((alpha >= 0) and ((alpha + beta) <=1)) do
															(
																CTVFace = getTVFace oMesh TIndices[i]
																-- get the TVerts of the face
																tv0 = (getTVert oMesh CTVFace.x)
																tv1 = (getTVert oMesh CTVFace.y)
																tv2 = (getTVert oMesh CTVFace.z)
																local tv = #(tv0, tv1, tv2)
																rUV = ((tv0 * (1 - (alpha + beta))) + (tv1 * alpha) + (tv2 * beta))
																rPoint = [P[1], P[2], P[3]]
																-- get the normal of the face
																rN = N
																rIndex = TIndices[i]
																fi = true
																if debug do format "FOUND INTERSECTION\n"
																return #(rPoint, rN, rIndex, rUV)
															)
													)
											) else (
												beta = ((v0 * u1) - (u0 * v1)) / ((v2 * u1) - (u2 * v1))
												if debug do format "Beta:%\n" beta
												if ((beta >= 0.0) and (beta <= 1.0)) do
													(
														alpha = (u0 - (beta * u2)) / u1
														if debug do format "Alpha:%\n" alpha
														if ((alpha >= 0) and ((alpha + beta) <=1)) do
															(
																CTVFace = getTVFace oMesh TIndices[i]
																-- get the TVerts of the face
																tv0 = (getTVert oMesh CTVFace.x)
																tv1 = (getTVert oMesh CTVFace.y)
																tv2 = (getTVert oMesh CTVFace.z)
																local tv = #(tv0, tv1, tv2)
																rUV = ((tv0 * (1 - (alpha + beta))) + (tv1 * alpha) + (tv2 * beta))
																rPoint = [P[1], P[2], P[3]]
																-- get the normal of the face
																rN = N
																rIndex = TIndices[i]
																fi = true
																if debug do format "FOUND INTERSECTION\n"
																return #(rPoint, rN, rIndex, rUV)
															)
													)
											)
									)
							)
					)
			)
		if fi == false do
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

Utility Caustics "Caustics"
	(

		local WaterObject, L, Terrain, CTexture,
					BrightenValue = 0.1				-- value that get's added to each pixel each time a ray hits it
	
		group "Debug Options"
			(
				checkbox debugMode "Debug Mode" checked:true
				checkbox GenerateRays "Generate Rays"
				checkbox GenerateTerrainInts "Generate Terrain Ints"
			)

		Group "New Bitmap Options"
			(
				spinner Width "Width:" range:[0,256,32] type:#integer
				spinner Height "Height:" range:[0,256,32] type:#integer
			)

		group "Refraction Parameters"
			(
				spinner sampleFreqX "Sample Freq X:" range:[1,2048,16] fieldwidth:35 type:#integer
				spinner sampleFreqY "Sample Freq Y:" range:[1,2048,16] fieldwidth:35 type:#integer
				spinner SourceRefraction "Source IoR:" range:[0,4,1.0] fieldwidth:35
				spinner DestRefraction "Destination IoR:" range:[0,4,1.33] fieldwidth:35
			)
		
		group "Image Filtering Options"
			(
				radiobuttons FilterMethod labels:#("No Filtering", "Barlett Window")
			)

		group "Animation"
			(
				checkbox AnimateCaustic "Animate" checked:true
				spinner StartFrame "Start Frame: " type:#integer range:[0,30,0]
				spinner EndFrame "End Frame: " type:#integer range:[0,30,30]
				spinner FrameStep "Frame Step: " type:#integer range:[1,100,1]
				editText SubStr "SubStr: " text:"MyBitmap"
			)

		label TerrainLabel "Terrain:" align:#left offset:[-10,0]
		pickbutton GetTerrain width:80 align:#right offset:[5,-20]
		label waterLabel "Water Object:" align:#left offset:[-10,0]
		pickbutton GetWater width:80 align:#right offset:[5,-20]
		label LightLabel "Light:" align:#left offset:[-10,0]
		pickbutton GetLight width:80 align:#right offset:[5,-20]

		label blank00
		button RenderBitmap "Render Caustic Map"

		label status1
		label status2
		
		on GetTerrain picked obj do
			(
				Terrain = obj
				GetTerrain.text = Terrain.name
			)

		on GetLight picked obj do
			(
				L = -(obj.dir) -- lights shine in the negative Z direction
				GetLight.text = obj.name
				format "Light Direction: %\n" L
			)

		on GetWater picked obj do
			(
				WaterObject = obj
				GetWater.text = WaterObject.name			
			)

		on RenderBitmap pressed do
			(
				-- determine the size of the water object
				if WaterObject != undefined then
					(
						if Terrain != undefined then
							(
								if animateCaustic.checked do
									(
										if (SavePath = (GetSavePath caption:"Give me path...now!") + "\\") != undefined do
											(
												if classof terrain.mat == standardMaterial do
													(
														if classof terrain.mat.diffusemap == bitmaptexture do
															CTexture = (openbitmap terrain.mat.diffusemap.filename)
													)
												-- format the start time to the listener window
												format "Start:%\n" localTime
		
												for t = StartFrame.value to EndFrame.value by FrameStep.value do
													(
														status2.text = ("t = " + t as string)
														gc()
														-- Generate a new bitmap of the size specified in the interface
														CausticMap = bitmap  width.value height.value
		
														if debugMode.checked do
															format "t = %\n" t

														-- Determine the scale of the terrain
														TerrainSize = (Terrain.max - Terrain.min)
														at time t
															(
																-- Determine the scale of the water
																WaterSize = (WaterObject.max - WaterObject.min)
		
																-- get the Caustic map array ready and filled with zeros
																CausticMapArray = #()
																for i = 1 to Height.value do
																	(
																		CausticMapArray[i] = #()
																		for j = 1 to width.value do
																			CausticMapArray[i][j] = 0.0
																	)
																YLimit = CausticMapArray.count
																XLimit = CausticMapArray[1].count
																CausticIndexSize = [(TerrainSize.x / width.value as float), (TerrainSize.y / Height.value as float)]
																InvCausticIndexSize = [(1 / CausticIndexSize.x), (1 / CausticIndexSize.y)]
																-- precalculate the index of refraction (n1/n2)
																Index = (SourceRefraction.value / DestRefraction.value)
		
																-- Create a ray
																r = ray [0,0,0] [0,0,-1]
		
																-- the PosZ value is precalculated for the z position of the ray, as it gets moved around
																PosZ = (WaterObject.max.z + 1.0)
		
																-- determine the pixel size and sampling width and height for
																-- stepping over the water object
																pixelsize = [(WaterSize.x / sampleFreqX.value), (WaterSize.y / sampleFreqY.value)]
																StartPosition = [(WaterObject.min.x + (pixelsize.x * 0.5)), (WaterObject.max.y - (pixelsize.x * 0.5))]
		
																-- print out some debug information to the Listener
																if debugMode.checked do
																	(
																		format "Water Scale: % % %\n" WaterSize.x WaterSize.y WaterSize.z
																		format "Pixel Size: % %\n" pixelsize.x pixelsize.y
																		format "Start Point: % %\n" StartPosition.x StartPosition.y
																		format "x:%\ny:%\n" (sampleFreqX.value - 1) (sampleFreqY.value - 1)
																	)
																-- now we start stepping through the water, calculating everything
																ProgressStart "Rendering Caustic Texture Map..."
																for x = 0 to (sampleFreqX.value - 1) do
																	(
																		status1.text = (x as string + " / " + (sampleFreqX.value - 1) as string)
																		ProgValue = (x / sampleFreqX.value) * 100
																		if (progressUpdate ProgValue) == false then Exit
																		for y = (sampleFreqY.value - 1) to 0 by -1 do
																			(
																				-- find the new position to look at
																				NewPos = [(StartPosition.x + (x * pixelsize.x)), (StartPosition.y - (y * pixelsize.y))]
																				-- move the ray there
																				r.pos = [NewPos.x, NewPos.y, PosZ]
																				-- find the intersect ray between the ray and the water
------------------------------------------------
																				if (ir = IntersectRay WaterObject r) != undefined then
--																			if (ir = Intersect r WaterObject) != undefined then
																					(
																						N = ir.dir
																						-- test to see if ir is facing away from the lightsource
																						-- if it is, we skip the rest of this and go on to the
																						-- next casting position
																						if (DotLN = (dot L N)) < 0.0 then
																							(
																								-- Index = n1/n2
																								sinTheta = Index * (sqrt (1 - (dot L N)^2))
																								-- q = (n1/n2 * L)
																								q = [(Index * L.x), (Index * L.y), (Index * L.z)]
																								Stuff = (sqrt(1 - sinTheta^2) + Index * DotLN)
																								TransRay = ray ir.pos (q - [(Stuff * N.x), (Stuff * N.y), (Stuff * N.z)])
																								if GenerateRays.checked do
																									(
																										c = cylinder()
																										c.pos = TransRay.pos
																										c.dir = TransRay.dir
																										c.height = 20
																										c.sides = 3
																										c.radius = 0.01
																									)
		
																								-- now that we have the transmitted ray,we intersect it with
																								-- the terrain to find out where it's hitting.  If it's undefined
																								-- we'll assume that it's refracting off the terrain surface.
-----------------------------------------------
																								if (tr = intersect TransRay Terrain) != undefined then
																									(
																										p3IntersectUV = tr[4]
																										iFaceIndex = tr[3]
																										iMatID = getFaceMatID terrain iFaceIndex
																										-- determine the pixel in the source texture we hit
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
-- ************************************************
																									) else (
																										if debugMode.checked do
																											format "Trans Ray undefined!!!\n"
																									)
																							) else (
																								if DebugMode.checked do
																									format "Skipped...\n"
																							)
																					) else (
																						format "undefined intersectRay!\n"
																					)
																			)
																	)
																ProgressEnd()
																for y = 1 to CausticMapArray.count do
																	(
																		Brightpoint = 0.0
		
																		for y = 1 to CausticMapArray.count do
																			for x = 1 to CausticMapArray[y].count do
																				if CausticMapArray[y][x] > BrightPoint do
																					BrightPoint = CausticMapArray[y][x]
		
																		BrightnessScalar = 1 / BrightPoint
		
																		CRow = #()
																		for x = 1 to CausticMapArray[y].count do
																			(
																				c = (CausticMapArray[y][x] * BrightnessScalar) * 255
																				if c > 255 do c = 255
																				c = (color c c c)
																				append CRow c
																			)
																		setpixels CausticMap [0,(y - 1)] CRow
																	)
															NewFileName = (SavePath + Substr.text + t as string + ".bmp")
															CausticMap.filename = NewFileName
															save CausticMap
															status1.text = ""

--															if debugMode.checked do
															display CausticMap
														)
												)
											format "End:%\n" localTime
											status1.text = ""
										)
								)
						) else (
							messageBox "Terrain object is not defined!!!"
						)
					) else (
						messageBox "Water object is not defined!!!"
					)
			)
	) -- end Utility