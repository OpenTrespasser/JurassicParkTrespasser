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

fn rndP3 p3 n = 
	(
		local mult = 10.0 ^ n
		nx = (floor ((p3.x * mult) + 0.5)) / mult
		ny = (floor ((p3.y * mult) + 0.5)) / mult
		nz = (floor ((p3.z * mult) + 0.5)) / mult
		return [nx,ny,nz]
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

fn LegalBitmap BMap = 
	(
		if BMap.width > 256 or Bmap.width < 8 or BMap.height > 256 or BMap.height < 8 then
			return false
		else
			return true
	)

Utility AssetDebugger "Asset Debugger"
(

	local		CObj, CMapArray, CMaterialArray, CMultiList, CMapList, PhotoshopFolder, BumpIndices,
					CenterPivotThresh = 0.10


	local debug = false
	
	pickButton PickObject "Pick Object" width:140
	checkbox CleanMesh "Clean mesh"
	group "General Attributes"
		(
			label Oname align:#left
			label OscaleX align:#left
			label OscaleY align:#left
			label OscaleZ align:#left
			label Oinstanced align:#left
			label OProps align:#left
		)

	group "Mesh Attributes"
		(
			label OVerts align:#left
			label OFaces align:#left
			label ODegenerate align:#left
			label ODegenerateNormals align:#left
			label ObadIDs align:#left
			label OBadUVWs align:#left
			label OPivot align:#left
			label HasOcclusion align:#left
			label StrayVerts align:#left	
		)

	group "Material/Map Attributes"
		(
			label MaterialType
			listbox CMaterial "Materials:" height:5
			label blank1
			label t "Tx:" align:#left offset:[-3,0]
			label o "Op:" align:#left offset:[-3,-3]
			label b "Bm:" align:#left offset:[-3,-3]
			listbox CMaps "Associated Bitmaps:" height:3 width:122 align:#right offset:[0,-65]
			button ViewMap "View Map" width:70 offset:[-37,0]
			button ViewPath "View Path" width:70 offset:[37,-26]
--		button ViewPhotoshop "View in Photoshop" width:140 enabled:false
			button getMapInfo "Get Bitmap Info" width:145
			label MapSize align:#left
			label MapMemoryUse align:#left
			label UnAssignedSubs align:#left
			label powerOfTwo align:#left
			label LegalBitmaps align:#left
			label TiledMaps align:#left
			label TiledBumpMaps align:#left
			label BitmapsOnNetwork align:#left
			label Ttb align:#left
		)

on getMapInfo pressed do
	(
		ObjArray = #()
		ObjArray = for o in objects collect o
		numObj = ObjArray.count
		
		for o in objArray do
			(
				format "Object: %\n" o.name
				if classof o.mat == standardmaterial do
					(
						if classof o.mat.diffusemap == bitmaptexture do (format "%\n" o.mat.diffusemap.filename)
						if classof o.mat.opacitymap == bitmaptexture do (format "%\n" o.mat.opacitymap.filename)
						if classof o.mat.bumpmap == bitmaptexture do (format "%\n" o.mat.bumpmap.filename)
					)
			
				if classof o.mat == multimaterial do
					(
						ns = o.mat.numsubs
						for i = 1 to ns do
							(
								if classof o.mat[i].diffusemap == bitmaptexture do (format "%\n" o.mat[i].diffusemap.filename)
								if classof o.mat[i].opacitymap == bitmaptexture do (format "%\n" o.mat[i].opacitymap.filename)
								if classof o.mat[i].bumpmap == bitmaptexture do (format "%\n" o.mat[i].bumpmap.filename)
							)
					)
			)
	)

on AssetDebugger open do
	(
--		if (getfiles "C:\debugdat.txt").count == 0 then
--			(
--				if (PhotoshopFolder = (GetSavePath caption:"Select the location of Photoshop")) != undefined do
--					(
--						f = createfile "C:\debugdat.txt"
--						openfile "debugdat.txt"
--						PhotoshopFolder = (PhotoshopFolder + "\\")
--						format ("Photoshop Location: " + PhotoshopFolder) to:f
--						close f
--						format "Photoshop Folder initialized to: %\n" PhotoshopFolder
--					)
--			) else (
--				f = openfile "C:\debugdat.txt"
--				CLine = readline f
--				for i = 1 to CLine.count do
--					(
--						if cLine[i] == ":" do
--							(
--								PhotoshopFolder = (substring CLine (i + 2) (CLine.count - i - 1))
--								format "Photoshop Location initialized to: %\n" PhotoshopFolder
--								Exit
--							)
--					)
--				close f
--			)
		1 == 1
	)

	on PickObject picked obj do
		(
			CMultiList = #()
			CMapList = #()
			CMapAray = #()
			CMapArray = #()
			CMat = -1
			CMap = -1
			CObj = obj
			ConvertToMesh CObj
			-- General Attributes

			OName.text = ("Name: " + CObj.name)
			CObjScale = (CObj.max - CObj.min)
			CObjScale.x = (roundTo CObjScale.x 2)
			CObjScale.y = (roundTo CObjScale.y 2)
			CObjScale.z = (roundTo CObjScale.z 2)
			OScaleX.text = ("Scale   x: " + CObjScale.x as string)
			OScaleY.text = ("            y: " + CObjScale.y as string)
			OScaleZ.text = ("            z: " +CObjScale.z as string)

			ObjName = Obj.name
			InstancedObject = false
			for i = 1 to ObjName.count do
				(
					if ObjName[i] == "-" do
						(
							InstancedObject = true
							Exit
						)
				)
			if InstancedObject == true then
				Oinstanced.text = "Instanced: Yes"
			else
				Oinstanced.text = "Instanced: No"

			TempBuffer = GetUserPropBuffer CObj
			if TempBuffer.count > 10 then
				OProps.text = "Obj Props Defined: Yes"
			else
				OProps.text = "Obj Props Defined: No"

			-- Mesh Attributes
			OVerts.text = ("Verts: " + CObj.numVerts as string)
			OFaces.text = ("Faces: " + CObj.numfaces as string)
			BadFaces = False
			df = #() -- Degenerate Faces
			for i = 1 to CObj.numfaces do
				(
					Cface = getface CObj i
					n = 4
					NewX = rndP3 (getvert CObj Cface.x) n
					NewY = rndP3 (getvert CObj Cface.y) n
					NewZ = rndP3 (getvert CObj Cface.z) n
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

--					if (colinear NewX NewY NewZ) do
--						(
--							if findItem df i == 0 do (append df i)
--							BadFaces = true
--						)
				)

			if BadFaces == false then
				ODegenerate.text = "Degenerate Polygons: No"
			else
				ODegenerate.text = "Degenerate Polygons: Yes"

			-- check for degenerate face normals
			BadFaces = false
			for i = 1 to CObj.numfaces do
				if (GetFaceNormal CObj i) == [0,0,0] do	
					(
						if findItem df i == 0 do (append df i)
						BadFaces = true
					)
			if BadFaces == false then
				ODegenerateNormals.text = "Degenerate Normals: No"
			else
				ODegenerateNormals.text = "Degenerate Normals: Yes"

			-- Test for BadUVWs
			BadUVs = false
			for i = 1 to CObj.numTVerts do
				(
					CTVert = (GetTVert CObj i).z
					if CTVert >= 0.0 and CTVert <= 0.0001 do BadUVs = true
				)

			for i = 1 to CObj.numTVerts do
				(
					CTVert = GetTVert CObj i
					if (abs CTVert.x) > 128 do BadUVWs = true
					if (abs CTVert.y) > 128 do BadUVWs = true
					if (abs CTVert.z) > 128 do BadUVWs = true
				)

			if BadUVs == true then
				(
					if CleanMesh.checked then
						(
							ClampW CObj
							addmodifier CObj (UVW_Cleanup())
							convertToMesh CObj
							OBadUVWs.text = "Possible Bad UV's: Fixed"
						) else (
							OBadUVWs.text = "Possible Bad UV's: Yes"
						)
				)
			else
				OBadUVWs.text = "Possible Bad UV's: No"

			if df.count > 0 do 
				(
					setFaceSelection CObj df
					update CObj
					if CleanMesh.checked do
						(
							df = getFaceSelection  CObj
							for i = df.count to 1 by -1 do
								deleteFace CObj df[i]
							update CObj
							ODegenerate.text = "Degenerate Polygons: Cleaned"
						)
				)

			BadMatIDs = false
			if classof CObj.mat == multiMaterial do
				(
					NumMats = CObj.mat.numsubs
					nf = CObj.numfaces
					BadMatIDs = false
					for i = 1 to nf do
						if (getfacematid CObj i) > NumMats do
							BadMatIDs = true
				)
			if classof CObj.mat == StandardMaterial do
				(
					nf = CObj.numfaces
					BadMatIDs = false
					for i = 1 to nf do
						if (getfacematid CObj i) != 1 do
							BadMatIDs = true
				)
			if BadMatIDs == false then
				ObadIDs.text = "Bad Material ID's: No"
			else
				ObadIDs.text = "Bad Material ID's: Yes"

			-- Test for centered pivot point
			CPivot = CObj.pivot
			CCenter = CObj.center
			if (distance CPivot CCenter) <= CenterPivotThresh then
				OPivot.text = "Centered pivot point: Yes"
			else 
				OPivot.text = "Centered pivot point: No"

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

			
			if NewVertSelection.count > 0 then
				(
					StrayVerts.text = "Stray Vertices: Yes"
					setvertselection CObj NewVertSelection
					updateCObj
					NewVertSelection = getVertSelection CObj
					if CleanMesh.checked do
						(
							for i = NewVertSelection.count to 1 by -1 do
								deleteVert CObj NewVertSelection[i]
							update CObj
							StrayVerts.text = "Stray Vertices: Cleaned"
						)
				) else (
					StrayVerts.text = "Stray Vertices: No"
				)
			-- Material/Map Attributes
			HasOcc = false

			if CObj.mat != undefined then
				(
					if classof CObj.mat == MultiMaterial do
						(
							BumpIndices = #()
							MaterialType.text = "Material Type: Multi"
							ns = CObj.mat.numsubs
		
							for i = 1 to ns do
								(
									append CMapArray #()
									append CMapList #()
									
									append CMultiList (i as string + ": " + CObj.mat[i].name)
									if (classof CObj.mat[i].diffusemap == bitmaptexture) and (CObj.mat[i].diffusemap.filename != "") then
										(
											CMapArray[i][1] = (openbitmap CObj.mat[i].diffusemap.filename)
											CMapList[i][1] = FileNamefromPath CObj.mat[i].diffusemap.filename
											if (CMapList[i][1] == "Soccludet2.bmp") or (CMapList[i][1] == "SOccludet2.bmp") do HasOcc = true
										) else (
											CMapArray[i][1] = undefined
											CMapList[i][1] = " "
										)
									if (classof CObj.mat[i].opacityMap == bitmaptexture) and (CObj.mat[i].OpacityMap.filename != "") then
										(
											CMapArray[i][2] = (openbitmap CObj.mat[i].OpacityMap.filename)
											CMapList[i][2] = FileNamefromPath CObj.mat[i].Opacitymap.filename
										) else (
											CMapArray[i][2] = undefined
											CMapList[i][2] = " "
										)
									if (classof CObj.mat[i].bumpmap == bitmaptexture) and (CObj.mat[i].bumpmap.filename != "") then
										(
											append BumpIndices i
											CMapArray[i][3] = (openbitmap CObj.mat[i].bumpmap.filename)
											CMapList[i][3] = FileNamefromPath CObj.mat[i].bumpmap.filename
										) else (
											CMapArray[i][3] = undefined
											CMapList[i][3] = " "
										)
								)
							CMaterial.items = CMultiList
							CMaps.items = CMapList[1]
						)
					if classof CObj.mat == StandardMaterial do
						(
							BumpIndices = #()
							MaterialType.text = "Material Type: Standard"
							append CMapArray #()
							append CMapList #()
							append CMultiList CObj.mat.name
							if (classof CObj.mat.diffusemap == bitmaptexture) and (CObj.mat.diffusemap.filename != "") then
								(
									CMapArray[1][1] = (openbitmap CObj.mat.diffusemap.filename)
									CMapList[1][1] = FileNamefromPath CObj.mat.diffusemap.filename
									if (CMapList[1][1] == "Soccludet2.bmp") or (CMapList[1][1] == "SOccludet2.bmp") do HasOcc = true
								) else (
									CMapArray[1][1] = undefined
									CMapList[1][1] = " "
								)
		
							if (classof CObj.mat.opacityMap == bitmaptexture) and (CObj.mat.opacityMap.filename != "") then
								(
									CMapArray[1][2] = (openbitmap CObj.mat.OpacityMap.filename)
									CMapList[1][2] = FileNamefromPath CObj.mat.Opacitymap.filename
								) else (
									CMapArray[1][2] = undefined
									CMapList[1][2] = " "
								)
		
							if (classof CObj.mat.bumpmap == bitmaptexture) and (CObj.mat.bumpmap.filename != "") then
								(
									BumpIndices = #(1)
									CMapArray[1][3] = (openbitmap CObj.mat.bumpmap.filename)
									CMapList[1][3] = FileNamefromPath CObj.mat.bumpmap.filename
								) else (
									CMapArray[1][3] = undefined
									CMapList[1][3] = " "
								)
							CMaterial.items = CMultiList
							CMaps.items = CMapList[1]
						)
					if CMapArray[1][1] != undefined then
						CMapSize = [CMapArray[1][1].width, CMapArray[1][1].height]
					else
						CMapSize = [0,0]
					MapSize.text = ("Width:" + (CMapSize.x as integer) as string + "          Height:" + (CMapSize.y as integer) as string)
					if HasOcc == true then
						HasOcclusion.text = "Uses Occlusion: Yes"
					else
						HasOcclusion.text = "Uses Occlusion: No"
				) else (
					MessageBox "No material defined"
				)

			CMemory = 0
			if CMapArray.count != 0 or CMapArray != undefined do
				(
					for i = 1 to CMapArray.count do
						(
							if CMapArray[i][1] != undefined then
								CMapSize = [CMapArray[i][1].width, CMapArray[i][1].height]
							else
								CMapSize = [0,0]
							-- test if bumpMap == true
							if CMapArray[i][3] != undefined then
								CMem = (CMapSize.x * CMapSize.y) * 4
							else 
								CMem = (CMapSize.x * CMapSize.y)
							CMemory = CMemory + CMem
						)
					if ((CMemory as float / 1024)/1024) >= 1.0 then
						MapMemoryUse.text = ("Total Memory Use: " + (roundTo ((CMemory / 1024.0)/1024) 2) as string + " MB")
					else
						MapMemoryUse.text = ("Total Memory Use: " + (roundTo (CMemory / 1024.0) 2) as string + " KB")
				)

			if CObj.mat != undefined do
				if classof CObj.mat == standardmaterial then
					(
						UnAssignedSubs.text = "Unassigned Subs: n/a"
					) else (
						if classof CObj.mat == MultiMaterial do
							(
								UnusedSub = false
								ns = CObj.mat.numsubs
								for i = 1 to ns do
									(
										for j = 1 to CObj.numfaces do
											(
												if i == (GetFaceMatID CObj j) then Exit
												if j == CObj.numfaces do UnusedSub = true
											)
										if UnusedSub == true then Exit
									)
								if UnusedSub == true then
									UnAssignedSubs.text = "Unassigned Subs: Yes"
								else
									UnAssignedSubs.text = "Unassigned Subs: No"
							)
					)
			if CMapArray != undefined do
				(
					Pof2 = true
					legal = true
					for i = 1 to CMapArray.count do
						for j = 1 to 3 do
							if CMapArray[i][j] != undefined do
								(
									if Power_of_two_Map CMapArray[i][j] != true do
										Pof2 = false
									if (LegalBitmap CMapArray[i][j]) == false do
										legal = false
								)
					if Pof2 == true then
						PowerOfTwo.text = "All maps power of 2: Yes"
					else
						PowerOfTwo.text = "All maps power of 2: No"

					if legal == true then
						LegalBitmaps.text = "All maps legal: Yes"
					else
						LegalBitmaps.text = "All maps legal: No"
					
				)
			
			if CObj.numTVerts != 0 then
				(
					TiledMap = false
					TiledBumpMap = false
					for i = 1 to CObj.numfaces do
						(
							CFace = getTVFace CObj i
							CTvert = gettvert CObj CFace.x
							if (CTvert.x < 0.0) or (CTvert.x > 1.0) do
								(
									TiledMap = true
									if classOf CObj.mat == standardmaterial do
										if BumpIndices.count > 0 do
											TiledBumpMap = true
									if classOf CObj.mat == multimaterial do
										if (findItem BumpIndices (GetFaceMatID CObj i)) != 0 do
											TiledBumpMap = true
								)
							if (CTvert.y < 0.0) or (CTvert.x > 1.0) do
								(
									TiledMap = true
									if classOf CObj.mat == standardmaterial do
										if BumpIndices.count > 0 do
											TiledBumpMap = true
									if classOf CObj.mat == multimaterial do
										if (findItem BumpIndices (GetFaceMatID CObj i)) != 0 do
											TiledBumpMap = true
								)
						
							CTvert = gettvert CObj CFace.y
							if (CTvert.x < 0.0) or (CTvert.x > 1.0) do
								(
									TiledMap = true
									if classOf CObj.mat == standardmaterial do
										if BumpIndices.count > 0 do
											TiledBumpMap = true
									if classOf CObj.mat == multimaterial do
										if (findItem BumpIndices (GetFaceMatID CObj i)) != 0 do
											TiledBumpMap = true
								)
							if (CTvert.y < 0.0) or (CTvert.x > 1.0) do
								(
									TiledMap = true
									if classOf CObj.mat == standardmaterial do
										if BumpIndices.count > 0 do
											TiledBumpMap = true
									if classOf CObj.mat == multimaterial do
										if (findItem BumpIndices (GetFaceMatID CObj i)) != 0 do
											TiledBumpMap = true
								)
		
							CTvert = gettvert CObj CFace.z
							if (CTvert.x < 0.0) or (CTvert.x > 1.0) do
								(
									TiledMap = true
									if classOf CObj.mat == standardmaterial do
										if BumpIndices.count > 0 do
											TiledBumpMap = true
									if classOf CObj.mat == multimaterial do
										if (findItem BumpIndices (GetFaceMatID CObj i)) != 0 do
											TiledBumpMap = true
								)
							if (CTvert.y < 0.0) or (CTvert.x > 1.0) do
								(
									TiledMap = true
									if classOf CObj.mat == standardmaterial do
										if BumpIndices.count > 0 do
											TiledBumpMap = true
									if classOf CObj.mat == multimaterial do
										if (findItem BumpIndices (GetFaceMatID CObj i)) != 0 do
											TiledBumpMap = true
								)
					)
			) else (
				MessageBox "This mesh does not have mapping coordinates!"
			)

			if TiledMap == true then
				tiledMaps.text = "Tiled Mapping: Yes"
			else
				tiledMaps.text = "Tiled Mapping: No"

			if TiledBumpMap == true then
				tiledBumpMaps.text = "Tiled Bump Mapping: Yes"
			else
				tiledBumpMaps.text = "Tiled Bump Mapping: No"

			MapsOnNet = true
			if CObj.mat != undefined do
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
					if MapsOnNet == true do
						BitmapsOnNetwork.text = "Map Locations on Net: Yes"
					if MapsOnNet == false do
						BitmapsOnNetwork.text = "Map Locations on Net: No"
				)

			-- Check for tTb cases
			DiffuseMapArray = #()
			MaterialArray = #()
			if classOf CObj.mat == standardmaterial do
				(
					CMat = #("","","")
					if classOf CObj.mat.diffusemap == bitmaptexture do
						CMat[1] = (FilenamefromPath CObj.mat.diffusemap.filename)
					if classOf CObj.mat.opacitymap == bitmaptexture do
						CMat[2] = (FilenamefromPath CObj.mat.opacitymap.filename)
					if classOf CObj.mat.bumpmap == bitmaptexture do
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

			if ModDiffuseArray.count >= 1 then
				ttb.text = "Invalid Map Assignments: Yes"
			else
				ttb.text = "Invalid Map Assignments: No"
		) -- End Pick Object

on ViewMap pressed do
	(
		CMat = CMaterial.selection
		CMap = CMaps.selection
		if CMapArray[CMat][CMap] != undefined do
			Display CMapArray[CMat][CMap]
	)

on ViewPath pressed do
	(
		CMat = CMaterial.selection
		CMap = CMaps.selection
		if CMapArray[CMat][CMap] != undefined do
			(
				CPath = CMapArray[CMat][CMap].filename
				MessageBox (CPath as string) title:"Map Path"
			)
	)

--	on ViewPhotoshop pressed do
--		(
--			CMat = CMaterial.selection
--			CMap = CMaps.selection
--			if CMapArray[CMat][CMap] != undefined do
--				doscommand (PhotoshopFolder + "photoshp.exe " + CMapArray[CMat][CMap].filename)
--				doscommand (PhotoshopFolder + "photoshp.exe")
--		)

on CMaterial selected i do
	(
		CMaps.items = CMapList[i]
		if CMapArray[i][1] != undefined then
			(
				if CMapArray[i][1] != undefined then
					CMapSize = [CMapArray[i][1].width, CMapArray[i][1].height]
				else
					CMapSize = [0,0]
					
				MapSize.text = ("Width:" + (CMapSize.x as integer) as string + "          Height:" + (CMapSize.y as integer) as string)
			) else (
				MapSize.text = "Width: -  Height: -"	
			)
	)
	
on CMaps selected j do
	(
		i = CMaterial.selection
		if CMapArray[i][j] != undefined then
			(
				if CMapArray[i][j] != undefined then
					CMapSize = [CMapArray[i][j].width, CMapArray[i][j].height]
				else
					CMapSize = [0,0]
				MapSize.text = ("Width:" + (CMapSize.x as integer) as string + "          Height:" + (CMapSize.y as integer) as string)
			) else (
				MapSize.text = "Width: -            Height: -"	
			)
	)
) -- End Utility