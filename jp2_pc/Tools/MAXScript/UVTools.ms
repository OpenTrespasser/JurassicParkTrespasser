-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *					Implementation of UVTools.ms
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************

fn UpdateUVsByFace obj FaceArray u v TVertData TVertList = 
	(
		vc = TVertData.count
		for i = 1 to vc do
			(
				local CTvert = [TVertData[i].x as float, TVertData[i].y as float, TVertData[i].z as float]
				setTvert obj TvertList[i] [CTvert.x + u, CTvert.y + v, CTvert.z]
			)
		Update obj
	)

fn ScaleUVsByFace Cobj FaceArray u v TVertData TVertIndices = 
	(
		vc = TVertData.count
		for i = 1 to vc do
			(
				local CTvert = [TVertData[i].x as float, TVertData[i].y as float, TVertData[i].z as float]
				CTvert.x = CTvert.x * u as float
				CTvert.y = CTvert.y * v as float
				setTvert Cobj TVertIndices[i] CTVert
			)
		Update Cobj
	)

fn RotateUV obj FaceArray angle TVertArray TVertList UShift VShift = 
	(
		UBase = 10000
		VBase = 10000
		UMax = -10000
		VMax = -10000
		
		for i = 1 to TVertArray.count do
			(
				local CTvert = TVertArray[i]
				if CTvert.x < UBase do UBase = CTvert.x
				if CTvert.y < VBase do VBase = CTvert.y
				if CTvert.x > UMax do UMax = CTvert.x
				if CTvert.y > VMax do VMax = CTvert.y
			)

		vc = TVertArray.count
		offsetX = (UBase + ((UMax - UBase) * 0.5))
		offsetY = (VBase + ((VMax - VBase) * 0.5))

		for i = 1 to vc do
			(
				CTvert = TVertArray[i]
				u = ((((CTVert.x - offsetX) * cos(angle)) - ((CTVert.y - offsetY) * sin(angle)))) + offsetX
				v = ((((CTVert.x - offsetX) * sin(angle)) + ((CTVert.y - offsetY) * cos(angle)))) + offsetY
				setTvert obj TvertList[i] [u, v, CTvert.z]
			)
		Update obj
	)

Utility UVTools "UV Tools"
(

	local FaceArray, TVertArray, TVertArrayA, RebuildUVs, RebuildUVsR, RebuildUVsS, CObj, OldU, OldV, NewNumU, NewNumV, OriginalTVertArray, NewScaleU, NewScaleV, TVertIndices, TVertData

	label ObjectLabel "Object:" align:#left
	pickbutton PickObject "" width:100 offset:[3,-20] align:#right
	radiobuttons FacesGroup labels:#("All Faces", "Face Selection")
	group ""
		(
			spinner ShiftU "Shift u:" range:[-100,100,0] type:#float offset:[0,0] fieldwidth:45 enabled:false
			spinner ShiftV "Shift v" range:[-100,100,0] type:#float fieldwidth:45 enabled:false
			Spinner RotateUVs "Rotation:" range:[-1800,1800,0] enabled:false fieldwidth:45
			spinner ScaleU "Scale u: " range:[0.01,1000,100] enabled:false fieldwidth:45
			spinner ScaleV "Scale v: " range:[0.01,1000,100] enabled:false fieldwidth:45
			checkbox LockScale "Lock Scale" align:#right enabled:false checked:true
		)

on facesGroup changed state do
	(
			RebuildUVs = true
			RebuildUVsR = true
			RebuildUVsS = true
			RotateUVs.value = 0
			ScaleU.value = ScaleV.value = 100
	)
			
-- ***************************************************************************************************
	on PickObject picked obj do
		(
			CObj = obj
			ConvertToMesh CObj
			PickObject.text = obj.name
			vertexArray = TVertArrayA = #()
			if FacesGroup.state == 1 then
				(
					fc = CObj.numfaces
					FaceArray = for i = 1 to fc collect i
				) else (
					FaceArray = getFaceSelection CObj
					fc = FaceArray.count
				)
			-- build a list of the texture vertices
			for i = 1 to fc do
				(
					CFace = getTVFace CObj FaceArray[i] -- returns a point3 of Texture Verts
					if finditem TVertArrayA Cface.x == 0 do (append TVertArrayA Cface.x)
					if finditem TVertArrayA Cface.y == 0 do (append TVertArrayA Cface.y)
					if finditem TVertArrayA Cface.z == 0 do (append TVertArrayA Cface.z)
				)
			TVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])
			OriginalTVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])
			ShiftU.value = ShiftV.value = 0.0
			NewScaleU = NewScaleV = NewNumU = NewNumV = 0
			ScaleU.value = ScaleV.value = 100
			RebuildUVs = true
			RebuildUVsR = true
			RebuildUVsS = true
			ShiftU.enabled = ShiftV.enabled = RotateUVs.enabled = ScaleU.enabled = ScaleV.enabled = LockScale.enabled = true
		)

-- ***************************************************************************************************
-- * Shifting UVs
-- ***************************************************************************************************

	on ShiftU changed state do
		(
			if CObj != undefined then
				(
					NewNumU = ShiftU.value
					if RebuildUVs == true do
						(
							vertexArray = TVertArrayA = #()
							if FacesGroup.state == 1 then
								(
									fc = CObj.numfaces
									FaceArray = for i = 1 to fc collect i
								) else (
									FaceArray = getFaceSelection CObj
									fc = FaceArray.count
								)
							-- build a list of the texture vertices
							for i = 1 to fc do
			 					(
									CFace = getTVFace CObj FaceArray[i] -- returns a point3 of Texture Verts
									if finditem TVertArrayA Cface.x == 0 do (append TVertArrayA Cface.x)
									if finditem TVertArrayA Cface.y == 0 do (append TVertArrayA Cface.y)
									if finditem TVertArrayA Cface.z == 0 do (append TVertArrayA Cface.z)
								)
							TVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])
							OriginalTVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])
							RebuildUVs = false
						)
					UpdateUVsByFace CObj FaceArray -NewNumU ShiftV.value TVertArray TVertArrayA
					RotateUVs.value = 0
					ScaleU.value = ScaleV.value = 100
					RebuildUVsR = true
					RebuildUVsS = true
			) else (
				MessageBox "No object defined"
				ShiftU.enabled = ShiftV.enabled = RotateUVs.enabled = ScaleU.enabled = ScaleV.enabled = LockScale.enabled = false
			)
		)
	
	on ShiftV changed state do
		(
			if CObj != undefined then
				(
					NewNumV = ShiftV.value
					if RebuildUVs == true do
						(
							vertexArray = TVertArrayA = #()
							if FacesGroup.state == 1 then
								(
									fc = CObj.numfaces
									FaceArray = for i = 1 to fc collect i
								) else (
									FaceArray = getFaceSelection CObj
									fc = FaceArray.count
								)
							-- build a list of the texture vertices
							for i = 1 to fc do
								(
									CFace = getTVFace CObj FaceArray[i] -- returns a point3 of Texture Verts
									if finditem TVertArrayA Cface.x == 0 do (append TVertArrayA Cface.x)
									if finditem TVertArrayA Cface.y == 0 do (append TVertArrayA Cface.y)
									if finditem TVertArrayA Cface.z == 0 do (append TVertArrayA Cface.z)
								)
							TVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])
							OriginalTVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])
							RebuildUVs = false
						)
					UpdateUVsByFace CObj FaceArray -ShiftU.value NewNumV TVertArray TVertArrayA
					RotateUVs.value = 0
					ScaleU.value = ScaleV.value = 100
					RebuildUVsR = true
					RebuildUVsS = true
			) else (
				MessageBox "No object defined"
				ShiftU.enabled = ShiftV.enabled = RotateUVs.enabled = ScaleU.enabled = ScaleV.enabled = LockScale.enabled = false
			)
		)

-- ***************************************************************************************************
-- * Rotating UVs
-- ***************************************************************************************************
	on RotateUVs changed state do
		(
			if CObj != undefined then
				(
					if RebuildUVsR == true do
						(
							format "Rebuilding Rotation UV's\n"
							vertexArray = TVertArrayA = #()
							if FacesGroup.state == 1 then
								(
									fc = CObj.numfaces
									FaceArray = for i = 1 to fc collect i
								) else (
									FaceArray = getFaceSelection CObj
									fc = FaceArray.count
								)
							-- build a list of the texture vertices
							for i = 1 to fc do
				 					(
									CFace = getTVFace CObj FaceArray[i] -- returns a point3 of Texture Verts
									if finditem TVertArrayA Cface.x == 0 do (append TVertArrayA Cface.x)
									if finditem TVertArrayA Cface.y == 0 do (append TVertArrayA Cface.y)
									if finditem TVertArrayA Cface.z == 0 do (append TVertArrayA Cface.z)
								)
							TVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])
						)
					RebuildUVsR = false
					RebuildUVsS = true
					RebuildUVs = true
					RotateUV CObj FaceArray RotateUVs.value TVertArray TVertArrayA ShiftU.value ShiftV.value
					ScaleU.value = ScaleV.value = 100
				) else (
					MessageBox "No object defined"
					ShiftU.enabled = ShiftV.enabled = RotateUVs.enabled = ScaleU.enabled = ScaleV.enabled = LockScale.enabled = false
				)
		)

-- ***************************************************************************************************
-- * Scaling UVs
-- ***************************************************************************************************
	on ScaleU changed state do
		(
			if CObj != undefined then
				(
					if LockScale.checked do
						ScaleV.value = ScaleU.value
					NewScaleU = ScaleU.value * 0.01
					NewScaleV = ScaleV.value * 0.01
					if RebuildUVsS == true do
						(
							format "Rebuilding Scale UV's\n"
							TVertIndices = #()
							if FacesGroup.state == 1 then
								(
									fc = CObj.numfaces
									FaceArray = for i = 1 to fc collect i
								) else (
									FaceArray = getFaceSelection CObj
									fc = FaceArray.count
								)
							-- build a list of the texture vertices
							for i = 1 to fc do
								(
									CFace = getTVFace CObj FaceArray[i] -- returns a point3 of Texture Verts
									if finditem TVertIndices Cface.x == 0 do (append TVertIndices Cface.x)
									if finditem TVertIndices Cface.y == 0 do (append TVertIndices Cface.y)
									if finditem TVertIndices Cface.z == 0 do (append TVertIndices Cface.z)
								)
							TVertData = for i = 1 to TVertIndices.count collect (getTVert CObj TVertIndices[i])
							RebuildUVsS = false
						)
					ScaleUVsByFace CObj FaceArray NewScaleU NewScaleV TVertData TVertIndices
					RotateUVs.value = 0
					RebuildUVsR = true
					RebuildUVs = true
				) else (
					MessageBox "No object defined"
					ShiftU.enabled = ShiftV.enabled = RotateUVs.enabled = ScaleU.enabled = ScaleV.enabled = LockScale.enabled = false
				)
		)

	on ScaleV changed state do
		(
			if CObj != undefined then
				(
					if LockScale.checked do
						ScaleU.value = ScaleV.value
					NewScaleU = ScaleU.value * 0.01
					NewScaleV = ScaleV.value * 0.01
					if RebuildUVsS == true do
						(
							format "Rebuilding Scale UV's\n"
							TVertIndices = #()
							if FacesGroup.state == 1 then
								(
									fc = CObj.numfaces
									FaceArray = for i = 1 to fc collect i
								) else (
									FaceArray = getFaceSelection CObj
									fc = FaceArray.count
								)
							-- build a list of the texture vertices
							for i = 1 to fc do
								(
									CFace = getTVFace CObj FaceArray[i] -- returns a point3 of Texture Verts
									if finditem TVertIndices Cface.x == 0 do (append TVertIndices Cface.x)
									if finditem TVertIndices Cface.y == 0 do (append TVertIndices Cface.y)
									if finditem TVertIndices Cface.z == 0 do (append TVertIndices Cface.z)
								)
							TVertData = for i = 1 to TVertIndices.count collect (getTVert CObj TVertIndices[i])
							RebuildUVsS = false
						)
					ScaleUVsByFace CObj FaceArray NewScaleU NewScaleV TVertData TVertIndices
					RotateUVs.value = 0
					RebuildUVsR = true
					RebuildUVs = true
				) else (
					MessageBox "No object defined"
					ShiftU.enabled = ShiftV.enabled = RotateUVs.enabled = ScaleU.enabled = ScaleV.enabled = LockScale.enabled = false
				)
		)
	)