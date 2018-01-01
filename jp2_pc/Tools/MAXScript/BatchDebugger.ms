fn MagV vec = (sqrt((vec.x)^2 + (vec.y)^2 + (vec.z)^2))

fn Colinear a b c = 
	if (MagV (cross (b - a) (c - a))) <= 0.001 then
		return true
	else
		return false

fn Thresh a b t =
	if (distance a  b) < t then
		return true
	else
		return false

fn ClampW CObj = 
	(
		for j = 1 to CObj.numTVerts do
			(
				c1 = getTvert CObj j
				if c1.z >= 0.0 and c1.z <= 0.0001 do SetTVert CObj j [c1.x, c1.y, 1]
			)
		update CObj
	)

fn CleanIDs o = 
	(
		if classof o.mat == Multimaterial do
			(
				ns = o.mat.numsubs
				for i = 1 to o.numfaces do
					if (iID = getFaceMatID o i) > ns do
						setFaceMatID o i (mod iID ns)
			)
		if classof o.mat == StandardMaterial do
			for i = 1 to o.numfaces do setFaceMatID o i 1
		update o
	)

fn MinMax foo a = 
	(
		mn = a[1]
		mx = a[1]
		for i = 2 to a.count do
			(
				if a[i] > mx do mx = a[i]
				if a[i] < mn do mn = a[i]
			)
		return #(mn, mx)
	)

fn detectStreakingFaces CObj = 
	(
		fs = #()
		local thresh = 0.00390625
		for i = 1 to CObj.numfaces do
			(
				CTVFace = getTVFace CObj i
				vx = getTvert CObj CTVFace.x
				vy = getTvert CObj CTVFace.y
				vz = getTvert CObj CTVFace.z
				du = minMax true #(vx.x, vy.x, vz.x)
				dv = minMax true #(vx.y, vy.y, vz.y)
				du = abs(du[1] - du[2])
				dv = abs(dv[1] - dv[2])
				if du < thresh or dv < thresh do append fs i
			)
		return fs
	)

fn LegalBitmap BMap = 
	(
		if BMap.width > 256 or Bmap.width < 8 or BMap.height > 256 or BMap.height < 8 then
			return false
		else
			return true
	)

fn ClampW CObj = 
	(
		for j = 1 to CObj.numTVerts do
			(
				c1 = getTvert CObj j
				if c1.z >= 0.0 and c1.z <= 0.0001 do SetTVert CObj j [c1.x, c1.y, 1]
			)
		update CObj
	)

fn ArrayCompare foo a b =
	(
		same = true
		for i = 1 to a.count do
			if a[i] != b[i] do same = false
		return same
	)

fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

fn Power_of_two_num num = 
	(
		validNum = false
		if num == 8 do (ValidNum = true)
		if num == 16 do (ValidNum = true)
		if num == 32 do (ValidNum = true)
		if num == 64 do (ValidNum = true)
		if num == 128 do (ValidNum = true)
		if num == 256 do (ValidNum = true)
		return ValidNum
	)

fn Power_of_two_Map BMap = 
	(
		ValidWidth = Power_of_two_num BMap.width
		ValidHeight = Power_of_two_num BMap.height
		if (validWidth == true) and (validHeight == true) then return true else return false
	)

Utility BatchDebugger "Batch Debugger"
	(
		local		CObj, GArray, CMapArray, CMaterialArray, CMultiList, CMapList, PhotoshopFolder, BumpIndices, CenterPivotThresh = 0.10

		local debug = false

		button ProcessSelection "Process Selection" width:140
		checkbox CleanMesh "Auto-Clean Meshes"
		label ObjCounter
		label foo
		group "Checklist Control"
			(
				button all "All" width:70 offset:[-37,0]
				button none "None" width:70 offset:[37,-26]
				checkbox DegenFaces "Degenerate Faces" checked:true
				checkbox DegenFaceNormals "Degenerate Face Normals" checked:true
				checkbox StreakingFaces "Streaking Faces" checked:true
				checkbox BadUVs "Bad UVs" checked:true
				checkbox BadMaterialIDs "Bad Material IDs" checked:true
				checkbox CenteredPivot "Centered Pivot Point" checked:true
				checkbox StrayVertices "Stray Vertices" checked:true
				checkbox Pof2MapSizes "Power of Two Map Sizes" checked:true
				checkbox LegalMapSizes "Legal Map Sizes" checked:true
				checkbox UnAssignedSubs "Unassigned SubMaterials" checked:true
				checkbox InvalidMapAssignments "Invalid Map Assignments" checked:true
				checkbox NetMapPaths "Maps on Network" checked:true
				checkbox tooManyProps "Too Many Props" checked:true
				
			)

		on all pressed do
			(
				DegenFaces.checked = true
				DegenFaceNormals.checked = true
				StreakingFaces.checked = true
				BadUVs.checked = true
				BadMaterialIDs.checked = true
				CenteredPivot.checked = true
				StrayVertices.checked = true
				Pof2MapSizes.checked = true
				LegalMapSizes.checked = true
				UnAssignedSubs.checked = true
				InvalidMapAssignments.checked = true
				NetMapPaths.checked = true
				TooManyProps.checked = true
			)

		on none pressed do
			(
				DegenFaces.checked = false
				DegenFaceNormals.checked = false
				StreakingFaces.checked = false
				BadUVs.checked = false
				BadMaterialIDs.checked = false
				CenteredPivot.checked = false
				StrayVertices.checked = false
				Pof2MapSizes.checked = false
				LegalMapSizes.checked = false
				UnAssignedSubs.checked = false
				InvalidMapAssignments.checked = false
				NetMapPaths.checked = false
				TooManyProps.checked = true
			)

		-- *******************************************************************
		-- * Batch Debug Selection
		-- *******************************************************************
		on ProcessSelection pressed do
			(
				ObjArray = selection as array
				GArray = geometry as array
				oc = ObjArray.count
				for i = 1 to oc do
					(
						if ObjArray[i].name[1] != "!" do
							if (findItem GArray ObjArray[i]) != 0 do
								(
									sErrorString = ""
									error = false
									ObjCounter.text = ("Object " + i as string + " of " + oc as string)
									CObj = ObjArray[i]
									-- does it have a material?
									if CObj.mat != undefined then
										hasMaterial = true
									else
										hasMaterial = false

									
									-- convert it to a mesh only if it has a material and also has UV's
									if hasMaterial then
										(
											ConvertToMesh CObj
											isMesh = true
										) else (
											isMesh = false
										)

									if isMesh then
										(
											if CObj.numTverts == 0 then
												hasUVs = false
											else
												hasUVs = true
										) else (
											hasUVs = false
										)

									sErrorString += ("\n*******************************************************\nObject: \"" + CObj.name + "\"\n")
									if hasUVs and hasMaterial and isMesh do
										if StreakingFaces.checked do
											if (sf = detectStreakingFaces CObj).count > 0 do
												(
													error = true
													sErrorString += ("Warning... " + sf.count as string + " streaking faces detected, please check visually...\n")
													if sf.count > 0 do setFaceSelection CObj sf
												)
									BadFaces = False
									df = #() -- Degenerate Faces
									-- *********************************************************
									-- * Degenerate Faces
									-- *********************************************************
									if isMesh do
											if DegenFaces.checked do
												(
													for i = 1 to CObj.numfaces do
														(
															Cface = getface CObj i
															NewX = getvert CObj Cface.x
															NewY = getvert CObj Cface.y
															NewZ = getvert CObj Cface.z
															if (Thresh NewX NewY 0.001) do 
																(
																	if findItem df i == 0 do (append df i)
																	BadFaces = true
																)
										
															if (Thresh NewX NewZ 0.001) do
																(
																	if findItem df i == 0 do (append df i)
																	BadFaces = true
																)
										
															if (Thresh NewZ NewY 0.001) do
																(
																	if findItem df i == 0 do (append df i)
																	BadFaces = true
																)
										
															-- if (colinear NewX NewY NewZ) do
															-- 	(
															-- 		if findItem df i == 0 do (append df i)
															-- 		BadFaces = true
															--	)
														)
													if BadFaces do
														(
															error = true
															sErrorString += "Has Degenerate Polygons\n"
														)
												)
						
									-- *********************************************************
									-- * Degenerate Face normals
									-- *********************************************************
									if isMesh do
										if DegenFaceNormals.checked do
											(
												-- check for degenerate face normals
												BadFaces = false
												for i = 1 to CObj.numfaces do
													if (GetFaceNormal CObj i) == [0,0,0] do	
														(
															if findItem df i == 0 do (append df i)
															BadFaces = true
														)
												if BadFaces do
													(
														error = true
														sErrorString += "Has Degenerate Face Normals\n"
													)
											)
				
	
									-- *********************************************************
									-- * Delete Degenerate Faces
									-- *********************************************************
									if isMesh do
										if df.count > 0 do 
											(
												setFaceSelection CObj df
												update CObj
												if CleanMesh.checked and (DegenFaces.checked or DegenFaceNormals.checked) do
													(
														error = true
														df = getFaceSelection  CObj
														for i = df.count to 1 by -1 do
															deleteFace CObj df[i]
														update CObj
														sErrorString += "Degenerate Polygons were Cleaned\n"
													)
											)

									-- *********************************************************
									-- * Bad UV coordinates
									-- *********************************************************
									if isMesh do
										if BadUVs.checked do
											(
												-- Test for BadUVWs
												BadUVWs = false
												if CObj.numTVerts > 0 then
													(
														for i = 1 to CObj.numTVerts do
															(
																CTVert = (GetTVert CObj i).z
																if CTVert >= 0.0 and CTVert <= 0.0001 do BadUVWs = true
															)
	
														for i = 1 to CObj.numTVerts do
															(
																CTVert = GetTVert CObj i
																if (abs CTVert.x) > 128 do BadUVWs = true
																if (abs CTVert.y) > 128 do BadUVWs = true
																if (abs CTVert.z) > 128 do BadUVWs = true
															)
								
														if BadUVWs do
															if CleanMesh.checked then
																(
																	error = true
																	ClampW CObj
																	addmodifier CObj (UVW_Cleanup())
																	convertToMesh CObj
																	sErrorString += "Bad UV's were Fixed\n"
																) else (
																	error = true
																	sErrorString += "Has Bad UV's\n"
																)
													) else (
														error = true
														sErrorString += "Does not have mapping coordinates...\n"
													)
											)
			
									-- *********************************************************
									-- * Bad Material IDs
									-- *********************************************************
									if BadMaterialIDs.checked do
										(
											if hasMaterial and isMesh do
												(
													BadMatIDs = false
													if classof CObj.mat == multiMaterial do
														(
															NumMats = CObj.mat.numsubs
															nf = CObj.numfaces
															BadMatIDs = false
															for i = 1 to nf do
																if (getfacematid CObj i) > NumMats do BadMatIDs = true
														)
													if classof CObj.mat == StandardMaterial do
														(
															nf = CObj.numfaces
															BadMatIDs = false
															for i = 1 to nf do
																if (getfacematid CObj i) != 1 do
																	BadMatIDs = true
														)
													if BadMatIDs == true do
														(
															error = true
															sErrorString += "Has Bad Material ID's\n"
														)
												)
										)
			
									-- *********************************************************
									-- * Centered Pivot Point
									-- *********************************************************
									if CenteredPivot.checked do
										(
											-- Test for centered pivot point
											CPivot = CObj.pivot
											CCenter = CObj.center
											if (distance CPivot CCenter) >= CenterPivotThresh do
												(
													error = true
													sErrorString += "Does not have it's pivot point centered\n"
												)
										)
	
									-- *********************************************************
									-- * Stray Vertices
									-- *********************************************************
									if StrayVertices.checked do
										(		
											if isMesh do
												(
													-- Find Stray Vertices
													StrayVertArray = (for i = 1 to CObj.numverts collect i)
													StrayVertArrayCopy = (for i = 1 to CObj.numverts collect i)
													UsedVerts = #()
													NewVertSelection = #()
							
													for i = 1 to CObj.numfaces do
														(
															CFace = (getface CObj i)
															if (findItem UsedVerts CFace.x) == 0 do (append UsedVerts CFace.x)
															if (findItem UsedVerts CFace.y) == 0 do (append UsedVerts CFace.y)
															if (findItem UsedVerts CFace.z) == 0 do (append UsedVerts CFace.z)
														)
							
													for i = 1 to StrayVertArray.count do
														if (findItem UsedVerts i) == 0 do (append NewVertSelection i)
							
													if NewVertSelection.count > 0 do
														(
															setvertselection CObj NewVertSelection
															updateCObj
															NewVertSelection = getVertSelection CObj
															if CleanMesh.checked then
																(
																	for i = NewVertSelection.count to 1 by -1 do deleteVert CObj NewVertSelection[i]
																	update CObj
																	error = true
																	sErrorString += "Stray Vertices were cleaned\n"
																) else (
																	error = true
																	sErrorString += "Has Stray Vertices\n"
																)
														)
												)
										)
												
									-- *********************************************************
									-- * Check Materials
									-- *********************************************************
									-- Check the materials
									CMapArray = #()
									CMapList = #()
									ns = -1
									if CObj.mat != undefined then
										(
											if classof CObj.mat == MultiMaterial do
												(
													BumpIndices = #()
													ns = CObj.mat.numsubs
													for j = 1 to ns do
														(
															append CMapArray #()
															append CMapList #()
			
															if CObj.mat[j] != undefined do
																(
																	if (classof CObj.mat[j].diffusemap == bitmaptexture) and (CObj.mat[j].diffusemap.filename != "") then
																		(
																			CMapArray[j][1] = (openbitmap CObj.mat[j].diffusemap.filename)
																			CMapList[j][1] = CObj.mat[j].diffusemap.filename
																		) else (
																			CMapArray[j][1] = undefined
																			CMapList[j][1] = " "
																		)
																	if (classof CObj.mat[j].opacityMap == bitmaptexture) and (CObj.mat[j].OpacityMap.filename != "") then
																		(
																			CMapArray[j][2] = (openbitmap CObj.mat[j].OpacityMap.filename)
																			CMapList[j][2] = CObj.mat[j].Opacitymap.filename
																		) else (
																			CMapArray[j][2] = undefined
																			CMapList[j][2] = " "
																		)
																	if (classof CObj.mat[j].bumpmap == bitmaptexture) and (CObj.mat[j].bumpmap.filename != "") then
																		(
																			append BumpIndices j
																			CMapArray[j][3] = (openbitmap CObj.mat[j].bumpmap.filename)
																			CMapList[j][3] = CObj.mat[j].bumpmap.filename
																		) else (
																			CMapArray[j][3] = undefined
																			CMapList[j][3] = " "
																		)
																)
														)
												)
											if classof CObj.mat == StandardMaterial do
												(
													ns = 0
													append CMapArray #()
													append CMapList #()
													if (classof CObj.mat.diffusemap == bitmaptexture) and (CObj.mat.diffusemap.filename != "") then
														(
															CMapArray[1][1] = (openbitmap CObj.mat.diffusemap.filename)
															CMapList[1][1] = CObj.mat.diffusemap.filename
														) else (
															CMapArray[1][1] = undefined
															CMapList[1][1] = " "
														)
													if (classof CObj.mat.opacityMap == bitmaptexture) and (CObj.mat.opacityMap.filename != "") then
														(
															CMapArray[1][2] = (openbitmap CObj.mat.OpacityMap.filename)
															CMapList[1][2] = CObj.mat.Opacitymap.filename
														) else (
															CMapArray[1][2] = undefined
															CMapList[1][2] = " "
														)
													if (classof CObj.mat.bumpmap == bitmaptexture) and (CObj.mat.bumpmap.filename != "") then
														(
															BumpIndices = #(1)
															CMapArray[1][3] = (openbitmap CObj.mat.bumpmap.filename)
															CMapList[1][3] = CObj.mat.bumpmap.filename
														) else (
															CMapArray[1][3] = undefined
															CMapList[1][3] = " "
														)
												)
											Pof2 = true
											for k = 1 to CMapArray.count do
												for m = 1 to 3 do
													if CMapArray[k][m] != undefined do
														(
															if Pof2MapSizes.checked do
																if (Power_of_two_Map CMapArray[k][m]) == false do
																	(
																		error = true
																		sErrorString += (CMapList[k][m] + "\n")
																	)
															if LegalMapSizes.checked do
																if (LegalBitmap CMapArray[k][m]) == false do
																	(
																		error = true
																		sErrorString += (CMapList[k][m] + "\n")
																	)
														)
										) else (
											error = true
											sErrorString += "Does not have a material defined\n"
										)
			
									-- *********************************************************
									-- * Check for unassigned Submaterials
									-- *********************************************************
									-- Check for Unassigned SubMaterials
									if UnAssignedSubs.checked do
										(
											if ns != -1 and isMesh do
												(
													UAS = false
													if ns == 0 then
														(
															for j = 1 to cObj.numfaces do
																if getFaceMatID CObj j != 1 do
																	UAS = true
														) else (
															for j = 1 to ns do
																(
																	for k = 1 to CObj.numfaces do
																		(
																			if j == (GetFaceMatID CObj k) then Exit
																			if k == CObj.numfaces do UAS = true
																		)
																	if UAS == true then Exit
																)
														)
													if UAS == true do
														(
															error = true
															sErrorString += "Has Unassigned Sub Materials\n"
														)
												)
										)
	
									-- *********************************************************
									-- * Check for Invalid Map Assignments
									-- *********************************************************
									-- Check for tTb cases
									if InvalidMapAssignments.checked do
										(
											if hasMaterial do
												(
													DiffuseMapArray = #()
													MaterialArray = #()
													if classOf CObj.mat == standardmaterial do
														(
															CMat = #("","","")
															if classOf CObj.mat.diffusemap == bitmaptexture and CObj.mat.diffusemap.filename != "" do
																CMat[1] = (FilenamefromPath CObj.mat.diffusemap.filename)
															if classOf CObj.mat.opacitymap == bitmaptexture and CObj.mat.opacitymap.filename != "" do
																CMat[2] = (FilenamefromPath CObj.mat.opacitymap.filename)
															if classOf CObj.mat.bumpmap == bitmaptexture and CObj.mat.bumpmap.filename != "" do
																CMat[3] = (FilenamefromPath CObj.mat.bumpmap.filename)
															found = false
															for j = 1 to MaterialArray.count do
																if (ArrayCompare true CMat MaterialArray[j]) == true do found = true
															if found == false do
																(
																	append DiffuseMapArray (FilenamefromPath CObj.mat.diffusemap.filename)
																	append MaterialArray CMat
																)
														)
													if classOf CObj.mat == multimaterial do
														(
															ns = CObj.mat.numsubs
															for i = 1 to ns do
																(
																	if CObj.mat[i] != undefined do
																		(
																			CMat = #("","","")
																			if classOf CObj.mat[i].diffusemap == bitmaptexture do
																				CMat[1] = (FilenamefromPath CObj.mat[i].diffusemap.filename)
																			if classOf CObj.mat[i].opacitymap == bitmaptexture do
																				CMat[2] = (FilenamefromPath CObj.mat[i].opacitymap.filename)
																			if classOf CObj.mat[i].bumpmap == bitmaptexture do
																				CMat[3] = (FilenamefromPath CObj.mat[i].bumpmap.filename)
																			found = false
																			for j = 1 to MaterialArray.count do
																				if (ArrayCompare true CMat MaterialArray[j]) == true do found = true
																			if found == false do
																				(
																					append DiffuseMapArray (FilenamefromPath CObj.mat[i].diffusemap.filename)
																					append MaterialArray CMat
																				)
																		)
																)
														)
										
													NewModDiffuseArray = #()
													ModDiffuseArray = #()
													ModMatArray = #()
													for i = 1 to DiffuseMapArray.count do	
														(
															CName = DiffuseMapArray[i]
															for j = 1 to DiffuseMapArray.count do
																if i != j do
																	if CName == DiffuseMapArray[j] do
																		(
																			append ModDiffuseArray CName
																			ModMatArray[ModDiffuseArray.count] = MaterialArray[i]
																		)
														)
										
													if ModDiffuseArray.count >= 1 do
														(
															error = true
															sErrorString += "Has Invalid Map Assignments\n"
														)
												)
										)
	
									-- *********************************************************
									-- * Check for Network Map Paths
									-- *********************************************************
									if NetMapPaths.checked do
										(
											MapsOnNet = true
											if hasMaterial do
												(
													if classof CObj.mat == standardmaterial do
														(
															if (classof CObj.mat.diffusemap == bitmaptexture) and (CObj.mat.diffusemap.filename != "") do
																if (CObj.mat.diffusemap.filename[1] == "k") == false and (CObj.mat.diffusemap.filename[1] == "K") == false and (CObj.mat.diffusemap.filename[1] == "\\") == false do 
																	MapsOnNet = false
															if (classof CObj.mat.opacitymap == bitmaptexture) and  (CObj.mat.opacitymap.filename != "") do 
																if (CObj.mat.opacitymap.filename[1] == "k") == false and (CObj.mat.opacitymap.filename[1] == "K") == false and (CObj.mat.opacitymap.filename[1] == "\\") == false do
																	MapsOnNet = false
															if (classof CObj.mat.bumpmap == bitmaptexture) and (CObj.mat.bumpmap.filename != "") do 
																if (CObj.mat.bumpmap.filename[1] == "k") == false and (CObj.mat.bumpmap.filename[1] == "K") == false and (CObj.mat.bumpmap.filename[1] == "\\") == false do
																	MapsOnNet = false
														)
			
													if classof CObj.mat == multimaterial do
														(
															ns = CObj.mat.numsubs
															for i = 1 to ns do
																(
																	if (classof CObj.mat[i].diffusemap == bitmaptexture) and (CObj.mat[i].diffusemap.filename != "") do 
																		if (CObj.mat[i].diffusemap.filename[1] == "k") == false and (CObj.mat[i].diffusemap.filename[1] == "K") == false and (CObj.mat[i].diffusemap.filename[1] == "\\") == false do
																			MapsOnNet = false
										
																	if (classof CObj.mat[i].opacitymap == bitmaptexture) and (CObj.mat[i].opacitymap.filename != "") do 
																		if (CObj.mat[i].opacitymap.filename[1] == "k") == false and (CObj.mat[i].opacitymap.filename[1] == "K") == false and (CObj.mat[i].opacitymap.filename[1] == "\\") == false do
																			MapsOnNet = false
										
																	if (classof CObj.mat[i].bumpmap == bitmaptexture) and (CObj.mat[i].bumpmap.filename != "") do 
																		if (CObj.mat[i].bumpmap.filename[1] == "k") == false and (CObj.mat[i].bumpmap.filename[1] == "K") == false and (CObj.mat[i].bumpmap.filename[1] == "\\") == false do
																			MapsOnNet = false
																)
														)
													if MapsOnNet == false do
														(
															error = true
															sErrorString += "Map Locations are not on the network\n"
														)
												)
										)

									if tooManyProps.checked do
										(
											CBuffer = getUserPropBuffer CObj
											if CBuffer.count > 500 do
												(
													error = true
													sErrorString += ("Has more than 500 characters in it's Text Properties buffer..." + (CBuffer.count as string) + " characters found.\n")
												)
										)

									sErrorString += "*******************************************************\n"
									if error == true do format "%" sErrorString
								)

					)
				ObjCounter.text = "Complete"
				messageBox "Complete.\nCheck the Listener Window for details"
			)
	) -- End Utility