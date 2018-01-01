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

fn RotateUV90 obj FaceArray angle TVertArray TVertList = 
	(
		vc = TVertArray.count
		offsetX = 0.5
		offsetY = 0.5
		for i = 1 to vc do
			(
				CTvert = TVertArray[i]
				u = ((((CTVert.x - offsetX) * cos(angle)) - ((CTVert.y - offsetY) * sin(angle)))) + offsetX
				v = ((((CTVert.x - offsetX) * sin(angle)) + ((CTVert.y - offsetY) * cos(angle)))) + offsetY
				setTvert obj TvertList[i] [u, v, CTvert.z]
			)
		Update obj
	)

fn FlipU obj FaceArray =
	(
		for i = 1 to FaceArray.count do
			(
				CTVFace = getTVFace obj FaceArray[i]
				-- get the u extents
				minU = (getTVert obj CTVFace.x).x
				if (getTVert obj CTVFace.y).x < minU do (minU = (getTVert obj CTVFace.y).x)
				if (getTVert obj CTVFace.z).x < minU do (minU = (getTVert obj CTVFace.z).x)

				maxU = (getTVert obj CTVFace.x).x
				if (getTVert obj CTVFace.y).x > maxU do (maxU = (getTVert obj CTVFace.y).x)
				if (getTVert obj CTVFace.z).x > maxU do (maxU = (getTVert obj CTVFace.z).x)
				
				Uextents = abs (maxU - minU)
				CTVert = getTVert obj CTVface.x
				setTvert obj CTVFace.x [abs (CTVert.x - Uextents), CTVert.y, CTVert.z]
				CTVert = getTVert obj CTVface.y
				setTvert obj CTVFace.y [abs (CTVert.x - Uextents), CTVert.y, CTVert.z]
				CTVert = getTVert obj CTVface.z
				setTvert obj CTVFace.z [abs (CTVert.x - Uextents), CTVert.y, CTVert.z]
				update obj
			)
	)

fn FlipV obj FaceArray =
	(
		for i = 1 to FaceArray.count do
			(
				CTVFace = getTVFace obj FaceArray[i]
				-- get the u extents
				minV = (getTVert obj CTVFace.x).y
				if (getTVert obj CTVFace.y).y < minV do (minV = (getTVert obj CTVFace.y).y)
				if (getTVert obj CTVFace.z).y < minV do (minV = (getTVert obj CTVFace.z).y)

				maxV = (getTVert obj CTVFace.x).y
				if (getTVert obj CTVFace.y).y > maxV do (maxV = (getTVert obj CTVFace.y).y)
				if (getTVert obj CTVFace.z).y > maxV do (maxV = (getTVert obj CTVFace.z).y)
				
				Vextents = abs (maxV - minV)
				CTVert = getTVert obj CTVface.x
				setTvert obj CTVFace.x [CTVert.x, abs (CTVert.y - Vextents), CTVert.z]
				CTVert = getTVert obj CTVface.y
				setTvert obj CTVFace.y [CTVert.x, abs (CTVert.y - Vextents), CTVert.z]
				CTVert = getTVert obj CTVface.z
				setTvert obj CTVFace.z [CTVert.x, abs (CTVert.y - Vextents), CTVert.z]
				update obj
			)
	)

Utility UVEdit "UV Edit"
	(
	
		local FaceArray, TVertArray, TVertArrayA, RebuildUVs, CObj
		
			label ObjLabel "Object: " align:#left
			pickbutton PickObject "" width:100 align:#right offset:[5,-20]
			button FlipTVertsU "Flip U" width:45 offset:[-50,0]
			button FlipTVertsV "Flip V" width:45 offset:[0,-26]
			button Rot90 "Rot 90" width:45 offset:[50,-26]
		
		on PickObject picked obj do
			(
				CObj = obj
				PickObject.text = obj.name
				vertexArray = TVertArrayA = #()
				FaceArray = getFaceSelection CObj
				fc = FaceArray.count
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
	
		on FlipTVertsU pressed do flipU CObj FaceArray
		on FlipTVertsV pressed do flipV CObj FaceArray

		on Rot90 pressed do
			(
				vertexArray = TVertArrayA = #()
				FaceArray = getFaceSelection CObj
				fc = FaceArray.count
				-- build a list of the texture vertices
				for i = 1 to fc do
					(
						CFace = getTVFace CObj FaceArray[i] -- returns a point3 of Texture Verts
						if finditem TVertArrayA Cface.x == 0 do (append TVertArrayA Cface.x)
						if finditem TVertArrayA Cface.y == 0 do (append TVertArrayA Cface.y)
						if finditem TVertArrayA Cface.z == 0 do (append TVertArrayA Cface.z)
					)
				TVertArray = for i = 1 to TVertArrayA.count collect (getTVert CObj TVertArrayA[i])

				RotateUV90 CObj FaceArray 90 TVertArray TVertArrayA
			)
	)

