-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *					Implementation of Slope.ms
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
fn rnd num = (num + 0.5 as integer)

Utility Slope "Slope"
	(
		local KeyObj, Terrain

		local Terrain2, NumMaps, MapPath

		label KeyLabel "Key:" align:#left
		pickbutton ChooseKey align:#right width:100 offset:[5,-20]
		label TerrainLabel "Terrain:" align:#left
		pickbutton ChooseTerrain align:#right width:100 offset:[5,-20]
		button UpdateAngle "Update Slope" width:140
		label angleLabel


		group "Selections"
			(
				spinner Thresh "Slope Threshold:" range:[1,89,45] fieldwidth:35
				button SelectFaces "Create Face Selection" width:140
			)

		group "Slope Maps"
			(
				label TerrainLabel2 "Terrain:" offset:[-5,0] align:#left
				pickbutton ChooseTerrain2 width:100 offset:[5,-20] align:#right
				spinner LowSlope "Low:" range:[0,90,29.9] fieldwidth:35 align:#left
				colorpicker LoColor color:(color 255 255 0) offset:[10,-22]
				spinner HighSlope "High:" range:[0,90,40] fieldwidth:35 align:#left
				colorpicker HiColor color:(color 255 0 0) offset:[10,-22]
				listbox DetailLevel "Number of Values:" height:3 items:#("256 Values", "128 Values", "64 Values", "32 Values", "16 Values", "8 Values", "4 Values") selection:2
				spinner Illum "Self Illum:" range:[0,100,0]
				button GenerateSlopeMaterial "Generate" width:140
				button SelfromSelection "Select from Selection"
				label betwn "angle is between slopes"
			)

		on ChooseTerrain2 picked obj do
			(
				terrain2 = obj
				ChooseTerrain2.text = terrain2.name
			)

		on SelfromSelection pressed do
			(
				if Terrain2 != undefined then
					(
						ObjArray = Selection as array
						sel = #()
						ProgressStart "Calculating Terrain Slope...."
						nfInv = (1.0 / ObjArray.count) * 100
						t = ray [0,0,0] [0,0,-1]
						for i = 1 to ObjArray.count do
							(
								progressUpdate (nfInv * i)
								t.pos = ObjArray[i].center
								ir = (intersectRay Terrain2 t)
								if ir != undefined do
									(
										N = ir.dir
										NewAngle = 90 - (atan (N.z / sqrt(N.x^2 + N.y^2)))
										if NewAngle < LowSlope.value do append sel ObjArray[i]
										if NewAngle > HighSlope.value do append sel ObjArray[i]
									)
							)
						ProgressEnd()
						select sel
					) else (
						MessageBox "Choose some terrain!"
					)
			)

		on GenerateSlopeMaterial pressed do
			(
				if Terrain2 != undefined then
					(
						if DetailLevel.selection == 1 do NumMaps = 256
						if DetailLevel.selection == 2 do NumMaps = 128
						if DetailLevel.selection == 3 do NumMaps = 64
						if DetailLevel.selection == 4 do NumMaps = 32
						if DetailLevel.selection == 5 do NumMaps = 16
						if DetailLevel.selection == 6 do NumMaps = 8
						if DetailLevel.selection == 7 do NumMaps = 4
						
						update Terrain2
						
						SlopeStep = 90.0 / NumMaps
						ColorStep = 255.0 / NumMaps
						
						-- Build Material
						NMat = Multimaterial()
						NMat.name = "Terrain Slope Material"
						NMat.numsubs = NumMaps + 2
						NumMapsInv = (1.0 / NumMaps) * 100
						ProgressStart "Generating New Material...."
						for i = 1 to (NumMaps + 2) do
							(
								progressUpdate (NumMapsInv * i)
								NMat[i] = standardMaterial()
								NMat[i].name = ("Value " + i as string)
								NMat[i].selfIllum = Illum.value
								NMat[i].shinestrength = 0
								NMat[i].shininess = 0
								if i >= (NumMaps + 1) then
									(
										if i == (NumMaps + 1) do
											c = LoColor.color
										if i == (NumMaps + 2) do
											c = HiColor.color
									) else (
										c = ((i - 1) * colorstep)
										c = (color c c c)
									)
								NMat[i].diffuse = c
							)
						ProgressEnd()
						
						ProgressStart "Calculating Terrain Slope...."
						nfInv = (1.0 / terrain2.numfaces) * 100
						for i = 1 to terrain2.numfaces do
							(
								progressUpdate (nfInv * i)
								CFace = getFace Terrain2 i
								N = getFaceNormal Terrain2 i
								NewAngle = 90 - (atan (N.z / sqrt(N.x^2 + N.y^2)))
								if NewAngle < LowSlope.value or NewAngle > HighSlope.value then
									(
										if NewAngle < LowSlope.value then
											NewMatID = NumMaps + 1
										else
											NewMatID = NumMaps + 2
									) else (
										NewMatID = rnd (NewAngle / SlopeStep)
									)
								if NewMatID > 256 do NewMatID = NumMaps
								if NewMatID < 1 do NewMatID = 1
								setFaceMatID Terrain2 i NewMatID
							)
						terrain2.mat = NMat
						ProgressEnd()
					) else (
						MessageBox "Choose some terrain!"
					)
			)

		on SelectFaces pressed do
			if Terrain != undefined do
				(
					FaceArray = #()
					ProgressStart "Generating Selection...."
					nf = Terrain.numfaces
					for i = 1 to nf do
						(
							progressUpdate ((i/(nf as float)) * 100)
							CFace = getFace Terrain i
							N = getFaceNormal Terrain i
							NewAngle = 90 - (atan (N.z / sqrt(N.x^2 + N.y^2)))
							if NewAngle >= Thresh.value do
								append FaceArray i
						)
					ProgressEnd()
					if FaceArray.count != 0 then
						setFaceSelection terrain FaceArray
					else
						messageBox "No faces selected"
				)

		on chooseKey picked obj do
			(
				KeyObj = obj
				ChooseKey.text = KeyObj.name
			)
	
		on chooseTerrain picked obj do
			(
				Terrain = obj
				ChooseTerrain.text = Terrain.name
			)	

		on UpdateAngle pressed do
			if KeyObj != undefined and Terrain != undefined do
				(
					r = ray [KeyObj.center.x, KeyObj.center.y, (Terrain.max.z + 1)] [0,0,-1]
					if (ir = intersectray Terrain r) != undefiend then
						(
							N = ir.dir
							NewAngle = 90 - (atan (N.z / sqrt(N.x^2 + N.y^2)))
							angleLabel.text = ("Slope: " + NewAngle as string)
						) else (
							angleLabel.text = "Slope: undefined"
						)
				)
	)