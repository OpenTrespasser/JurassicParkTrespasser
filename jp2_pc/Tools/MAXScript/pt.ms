-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of PaintTrees.ms 
-- *
-- * Bugs:
-- * 
-- * To do:
-- * 
-- * Notes:
-- * 
-- **********************************************************************************************
fn GetTverts CObj =
	(
		nf = CObj.numfaces
		TVertArray = #()
		TVertUVArray = #()
		TVFaceArray = #()
		
		local FaceArray = getFaceSelection CObj
		-- if we get nothing back from the face selection, assume that we want all the faces
		if FaceArray.count == 0 do
			FaceArray = for i = 1 to nf collect i
		
		-- get all the Tverts
		for i = 1 to FaceArray.count do
			append TVFaceArray (getTVFace CObj FaceArray[i])
		
		for i = 1 to TVFaceArray.count do
			(
				if FindItem TVertArray TVFaceArray[i].x == 0 do (append TVertArray (TVFaceArray[i].x as integer))
				if FindItem TVertArray TVFaceArray[i].y == 0 do (append TVertArray (TVFaceArray[i].y as integer))
				if FindItem TVertArray TVFaceArray[i].z == 0 do (append TVertArray (TVFaceArray[i].z as integer))
			)
		-- get the TVert point3's
		TVertUVArray = (for i = 1 to TVertArray.count collect (GetTVert CObj TVertArray[i]))

		AllTVerts = #(TVertArray, TVertUVArray)
		return AllTVerts
	)


fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

Utility PaintTrees "Paint Trees"
(

local 	width, height, Terrain, Tnv, nh, TotalWorldSpace, DistObjectDimension,
				veg1, veg2, Veg1Radius, Veg2Radius, BitMapScale, VegRadiusBackup,
				PosRay = ray [0,0,0] [0,0,-1],
				OverlapAllowence = 0.75,
				PlacementArray = #(),
				PositionList = #(),
				ScaleBitmapArray = #(),
				ScaleLumArray = #(),
				DensityBitmapArray = #(),
				DensityLumArray = #(),
				DistBitmapArray = #(),
				DistLumArray = #(),
				ScaleDistribution = false,
				DensityDistribution = false,
				RandomOffset = 1.0

checkBox DebugInfo "Debug Mode" checked:true align:#center

group "Initialization Options"
	(
		button SelAware "Initialize Aware Objects"
		checkbox TreeAware "Aware objects are trees"
		spinner TreeThreshold "Threshold:" range:[0.1, 3, 0.5] type:#float
		label PosListLabel
	)

group "Bitmap Options"
	(
		button getDistributionBitmap "" width:100 offset:[20,0]
		label DistributionBitmaplabel "Place:" offset:[-55,-20]
		label bitmapInfo1
		button getScaleBitmap "" width:100 offset:[20,0]
		label ScaleBitmaplabel "Scale:" offset:[-55,-20]
		spinner ScaleThreshold1 "Thresh:" range:[0,1000,90] type:#float fieldwidth:40 offset:[-55,0] enabled:false
		spinner ScaleThreshold2 "--" range:[0,1000,110] type:#float fieldwidth:40 offset:[3,-21] enabled:false
		label ScalebitmapInfo1
		button getDensityBitmap "" width:100 offset:[20,0]
		label DensityBitmaplabel "Density:" offset:[-55,-20]
		spinner DensityThreshold1 "Thresh:" range:[0.1,5,0.5] type:#float fieldwidth:40 offset:[-55,0] enabled:false
		spinner DensityThreshold2 "--" range:[0.1,5,2] type:#float fieldwidth:40 offset:[3,-21] enabled:false
		label DensitybitmapInfo1
	)

group "Distribution Options"
	(
		label targetObjlabel "Terrain:" offset:[-55,0]
		pickbutton picktarget "" width:100 offset:[20,-20]
		pickbutton pickDistributionObject1 "" width:100 offset:[20,0]
		label DistributionObjectlabel "Object:" offset:[-55,-21]
		label filler
		spinner LowValueThreshold1 "Thresh:" range:[0,255,127] type:#integer fieldwidth:35 offset:[-57,0]
		spinner HiValueThreshold1 "to:" range:[0,255,255] type:#integer fieldwidth:35 offset:[3,-21]
		spinner Overlap "Density Threshold:" range:[0.1,5,1.0] type:#float fieldwidth:35
		spinner RndOffset "Random Offset:" range:[0,10,1] type:#float fieldwidth:35
		radiobuttons CloneAs "Clone as:" labels:#("Copy", "Instance") align:#left
		checkbox randomRotate "Random rotate"
	)

button distributeObjects "Distribute Objects" width:140
label status1

label help1 "Feed me RGB Maps only!!!"
label help2 "(not gray scale  or 8-bit)"

on Overlap changed state do
	OverlapAllowence = Overlap.value

on RndOffset changed state do
	randomOffset = RndOffset.value

on SelAware pressed do
	(
		PositionList = #()
		ObjArray = for obj in selection collect obj
		if TreeAware.checked then
			(
				for i = 1 to ObjArray.count do
					(
						ValidVerts = #()
						CObj = ObjArray[i]
						LowVert = (GetVert CObj 1).z
						VertArray = for i = 1 to CObj.numverts collect (getVert CObj i)
						-- find the lowest vert in the object
						for i = 1 to VertArray.count do
							if VertArray[i].z < LowVert do (LowVert = VertArray[i].z)
						-- Find all the verts within the vertical threshold, and append them to ValidVerts
						for i = 1 to VertArray.count do
							if abs(LowVert - VertArray[i].z) <= TreeThreshold.value do
								append ValidVerts VertArray[i]
							
						-- get the box extents of the new set of verts
						MaxX = ValidVerts[1].x
						MaxY = ValidVerts[1].y
						MaxZ = ValidVerts[1].z

						MinX = ValidVerts[1].x
						MinY = ValidVerts[1].y
						MinZ = ValidVerts[1].z
						
						for i = 2 to ValidVerts.count do
							(
								if ValidVerts[i].x > MaxX do (MaxX = ValidVerts[i].x)
								if ValidVerts[i].y > MaxY do (MaxY = ValidVerts[i].y)
								if ValidVerts[i].z > MaxZ do (MaxZ = ValidVerts[i].z)

								if ValidVerts[i].x < MinX do (MinX = ValidVerts[i].x)
								if ValidVerts[i].y < MinY do (MinY = ValidVerts[i].y)
								if ValidVerts[i].z < MinZ do (MinZ = ValidVerts[i].z)
							)
						VertsMax = [MaxX, MaxY, MaxZ]
						VertsMin = [MinX, MinY, MinZ]
						VertsScale = (VertsMax - VertsMin)
						VertsRadius = VertsScale.x
						if VertsScale.y > VertsRadius do (VertsRadius = VertsScale.y)
						VertsRadius = VertsRadius * 0.5
						format "Verts Radius: %\n" VertsRadius
						append PositionList #([CObj.pos.x, CObj.pos.y], VertsRadius)
					)
				PosListLabel.text = (PositionList.count as string + " objects initialized")
			) else (

				for i = 1 to ObjArray.count do
					(
						CObj = ObjArray[i]
						CObjScale = (CObj.max - CObj.min)
						CObjRadius = CObjScale.x
						if cObjScale.y > CObjRadius do CObjRadius = CObjScale.y
						CObjRadius = CObjRadius * 0.5
						append PositionList #([CObj.pos.x, CObj.pos.y], CObjRadius)
					)
				PosListLabel.text = (PositionList.count as string + " objects initialized")
			)
	)

on LowValueThreshold1 changed state do
	if LowValueThreshold1.value >= HiValueThreshold1.value do
		HiValueThreshold1.value = (LowValueThreshold1.value + 1)

on HiValueThreshold1 changed state do
	if HiValueThreshold1.value <= LowValueThreshold1.value do
		LowValueThreshold1.value = (HiValueThreshold1.value - 1)

on distributeObjects pressed do
(
	if Terrain != undefined then
		(
			ProgressStart ""
			PixelDimension = [(DistLumArray[1].count / TotalWorldSpace.x) * .1, (DistLumArray.count / TotalWorldSpace.y) * .1]
			if DebugInfo.checked do
				format "Pixel Dimension: %\n" PixelDimension
			opc = 0
			oc = 0
			for y = 1 to DistLumArray.count do
				(
					ProgressUpdate ((y as float/ DistLumArray.count) * 100)
					for x = 1 to DistLumArray[y].count do
						(
							if PlacementArray[y][x] == #free do
								(
									if (DistLumArray[y][x] >= LowValueThreshold1.value) and (DistLumArray[y][x] <= HiValueThreshold1.value) do
										(
											Veg1Radius = VegRadiusBackup as float
											CPixel = [x,y]
											WorldPos = [(((CPixel.x * TotalWorldSpace.x) / DistLumArray[y].count) + Terrain.min.x), (((CPixel.y * TotalWorldSpace.y) / DistLumArray.count) + Terrain.min.y)]

											ExitCode = 0
											if DensityDistribution == true do
												(
													DRange = (DensityThreshold1.value - DensityThreshold2.value) / 255
													OverlapAllowence = ((DRange * DensityLumArray[y][x]) + DensityThreshold2.value)
												)
											if ScaleDistribution == true do
												(
													NewScale = 1.0
													range = (ScaleThreshold2.value - ScaleThreshold1.value) / 255
													NewScale = ((range * ScaleLumArray[y][x]) + ScaleThreshold1.value) * 0.01
													Veg1Radius = Veg1Radius * NewScale
												)

											theta = random 0.0 360.0
											radius = (random Veg1Radius 0) * RandomOffset
											xPos = radius * (cos(theta))
											yPos = radius * (sin(theta))
											WorldPos += [xPos,yPos,0]
											
											if PositionList.count != 0 do
												(
													for i = 1 to PositionList.count do
														(
															CombinedRadii = ((Veg1Radius + PositionList[i][2]) * OverlapAllowence)
															if (distance WorldPos PositionList[i][1]) < CombinedRadii do
																(
																	ExitCode = 1
																	Exit
																)
														)
												)
											if ExitCode == 0 do
												(
													if CloneAs.state == 1 then
														(
															TempObj = copy Veg1
															oc = oc + 1
														) else (
															TempObj = instance Veg1
															oc = oc + 1
														)

													if ScaleDistribution == true do
														TempObj.scale = TempObj.scale * NewScale

													TempObj.pos = [(WorldPos.x - PixelDimension.x * 0.5), (WorldPos.y - PixelDimension.y * 0.5), 0]

													append PositionList #([TempObj.pos.x, TempObj.pos.y], Veg1Radius)

													if RandomRotate.checked do (rotate TempObj (random -179.5 179.5) Z_axis)
												)
										)
								)
						)
				)
			-- test for intersections
			ProgressEnd()
			status1.text = (oc as string + " objects created")
		) else (
			messageBox "Target Object not defined!"
		)
)

	on pickDistributionObject1 picked obj do
		(
			Veg1 = obj
			DistObjectDimension1 = (Veg1.max - Veg1.min)
			pickDistributionObject1.text = Veg1.name
			Veg1Radius = DistObjectDimension1.x
			if DistObjectDimension1.y > Veg1Radius do
				Veg1Radius = DistObjectDimension1.y
			Veg1Radius = Veg1Radius * 0.5
			VegRadiusBackup = Veg1Radius as float
			if debugInfo.checked do
				format "Veg1 Radius: %\n" Veg1Radius
		)

	on pickTarget picked obj do
		(
			Terrain = obj
			ConvertToMesh Terrain
			TotalWorldSpace = (Terrain.max - Terrain.min)
			if DebugInfo.checked do
				format "Total World space:%\n" TotalWorldSpace
			pickTarget.text = Terrain.name
		)

	on getDistributionBitmap pressed do
		(
			DistributionBitmap = selectbitmap caption:"Choose Object Distribution Bitmap"
			if DistributionBitmap != undefined do
				(
					DistBitmapArray = #()
					DistLumArray = #()
					PlacementArray = #()
					width = DistributionBitmap.width
					height = DistributionBitmap.height

					if BitMapScale == undefined do
						BitMapScale = [width, height]

					if BitMapScale == [width, height] then
						(
							getDistributionBitmap.text = filenameFromPath DistributionBitmap.filename
							for i = 1 to height do 
								(
									DistBitmapArray[i] = #()
									DistLumArray[i] = #()
									PlacementArray[i] = #()
								)
							for y = 1 to height do
								DistBitmapArray[y] = getpixels DistributionBitmap [0,(height - y)] width
							DistLumArray = DistBitmapArray
							For y = 1 to DistLumArray.count do
								for x = 1 to Width do
									(
										DistLumArray[y][x] = DistBitmapArray[y][x].value
										PlacementArray[y][x] = #free
									)
							bitmapInfo1.text = ("Width: " + width as string + "     Height: " + height as string)
						) else (
							MessageBox "All bitmaps must be of the same dimension!"
							DistributionBitmap = undefined
						)
				)
		)

	on getScaleBitmap pressed do
		(
			ScaleBitmap = selectbitmap caption:"Choose Object Scaling Bitmap"
			if ScaleBitmap != undefined do
				(
					ScaleBitmapArray = #()
					ScaleLumArray = #()
					width = ScaleBitmap.width
					height = ScaleBitmap.height

					if BitMapScale == undefined do
						BitMapScale = [width, height]

					if BitMapScale == [width, height] then
						(
							getScaleBitmap.text = filenameFromPath ScaleBitmap.filename
							for i = 1 to height do 
								(
									ScaleBitmapArray[i] = #()
									ScaleLumArray[i] = #()
								)
							for y = 1 to height do
								ScaleBitmapArray[y] = getpixels ScaleBitmap [0,(height - y)] width
							ScaleLumArray = ScaleBitmapArray
							For y = 1 to ScaleLumArray.count do
								for x = 1 to Width do
									ScaleLumArray[y][x] = ScaleBitmapArray[y][x].value
							ScalebitmapInfo1.text = ("Width: " + width as string + "     Height: " + height as string)
							ScaleThreshold1.enabled = ScaleThreshold2.enabled = true
							ScaleDistribution = true
						) else (
							MessageBox "All bitmaps must be of the same dimension!"
							ScaleBitmap = undefined
						)
				)
		)


	on getDensityBitmap pressed do
		(
			DensityBitmap = selectbitmap caption:"Choose Object Density Bitmap"
			if DensityBitmap != undefined do
				(
					DensityBitmapArray = #()
					DensityLumArray = #()
					getDensityBitmap.text = filenameFromPath DensityBitmap.filename
					width = DensityBitmap.width
					height = DensityBitmap.height

					if BitMapScale == undefined do
						BitMapScale = [width, height]

					if BitMapScale == [width, height] then
						(
							for i = 1 to height do 
								(
									DensityBitmapArray[i] = #()
									DensityLumArray[i] = #()
								)
							for y = 1 to height do
								DensityBitmapArray[y] = getpixels DensityBitmap [0,(height - y)] width
							DensityLumArray = DensityBitmapArray
							For y = 1 to DensityLumArray.count do
								for x = 1 to Width do
									DensityLumArray[y][x] = DensityBitmapArray[y][x].value
							DensitybitmapInfo1.text = ("Width: " + width as string + "     Height: " + height as string)
							DensityThreshold1.enabled = DensityThreshold2.enabled = true
							DensityDistribution = true
							Overlap.enabled = false
						) else (
							MessageBox "All bitmaps must be of the same dimension!"
							DensityBitmap = undefined
						)
				)
		)


)-- end Utility
