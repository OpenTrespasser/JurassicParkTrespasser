-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of TreePlacement.ms
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
fn TerrainObjectFlt obj = findstring obj.name "!Terrain" == 1

fn LowestVert obj =
	(
		-- returns the index of the vertex with the lowest pos.z value
		local LowVertPos = obj.max.z
		local LowestVert = 1
		nv = obj.numverts
		for i = 1 to nv do
			(	
				local Cvert = getvert obj i
				if Cvert.z < LowVertPos do
					(
						LowVertPos = Cvert.z
						LowestVert = i
					)
			)	
		return LowestVert
	)

fn LowestVerts obj thresh =
	(
		-- returns the index of the vertex with the lowest pos.z value
		local LowVertPos = obj.max.z
		local LowestVert = 1
		nv = obj.numverts
		for i = 1 to nv do
			(	
				local Cvert = getvert obj i
				if Cvert.z < LowVertPos do
					(
						LowVertPos = Cvert.z
						LowestVert = i
					)
			)
		LowVertThreshold = LowVertPos + thresh
		LowVerts = #()
		for i = 1 to nv do
			(	
				local Cvert = getvert obj i
				if Cvert.z < LowVertThreshold do
					append LowVerts i
			)
		return LowVerts
	)

-- **********************************************************************************************
-- Start Utility
-- **********************************************************************************************

Utility Tree_Debris_Placer "Tree Placer"
	(
	
		local Terrain_surface, Tapeobj, tree, taperay, ir, ParentObject, Debris1Obj, Debris2Obj, 
		Debris3Obj, Debris4Obj, InitialDebrisPos, NewDebris, ParentObject, PlacedObjCount, 
		TotalPolyCount, SubPolyCount1, SubPolyCount2, SubPolyCount3, SubPolyCount4, NumSelected,DebrisOptions1,
		DebrisOptions2, DebrisOptions3, DebrisOptions4, DebrisOptions5, DebrisOptions6, CWireColor
		
		local debug = false
		
		group "Distribution Options"
			(
				label chooseTerrainLabel "Terrain:" align:#left offset:[-5,0]
				pickbutton Pick_Terrain width:100 align:#right offset:[5,-20] filter:TerrainObjectFlt
				label chooseSurfLabel "Surface:" align:#left offset:[-5,0]
				pickbutton Pick_PlaceSurface width:100 align:#right offset:[5,-20]
				checkbox RotSel "Random Z Rotation" checked:false
				spinner ZrotHi "Hi:" range:[0,180,30] enabled:false fieldwidth:45 offset:[-5,0] align:#left
				spinner ZrotLo "Lo:" range:[-180,0,-30] enabled:false fieldwidth:45 offset:[5,-21]
				checkbox RndTilt "Random Tilt" enabled:false
				spinner tilt "Tilt up to:" range:[0,30,0] type:#float enabled:false fieldwidth:40 align:#left
				label tiltlabel "degrees" align:#right offset:[0,-20]
				checkbox RndScale "Random Scale"
				spinner rndScaleLo "Low %:" range:[.001,1000,75] type:#float enabled:false
				spinner rndScaleHi "High %:" range:[.001,1000,150] type:#float enabled:false
				spinner HeightOffset "Height Offset: " range:[-10,10,0] type: #float
			)
		
		group "Placement Method"
			(
				radiobuttons PlacementMethod labels:#("Box", "Vert","Normal","Boulder", "Planted") columns:2
				spinner VertexThreshold "Vert Threshold:" range:[0,10,.25] enabled:false fieldwidth:45
			)
		
		group "Conforming Options"
			(
				checkbox conformSelected "Conform Selected Verts"
				checkbox ApplyRenaming "Apply Renaming" checked:true offset:[15,0]
				checkbox ConformOnly "ConformOnly" checked:false offset:[15,0]
			)

		group "Direction Options"
			(
				checkbox OverrideDirection "Override Direction"
				spinner DirX "X:" range:[-1,1,0] enabled:false
				spinner DirY "Y:" range:[-1,1,0] enabled:false
				spinner DirZ "Z:" range:[-1,1,-1] enabled:false
				label blank0
				button PresetX "X" width:25 offset:[-45,-80] enabled:false
				button PresetY "Y" width:25 offset:[-45,0] enabled:false
				button PresetZ "Z" widht:25 offset:[-45,0] enabled:false
				checkbox negative "(-)" offset:[10,0] enabled:false
				label blank1
				label ObjDir "Picked Dir:" align:#left offset:[-5,0] enabled:false
				pickbutton PickObjDir width:80 align:#right offset:[5,-20] enabled:false
			)

		button place_many "Place Selection" width:145
		checkbox SmartMode "Smart Mode"
		label results " " align: #left

		on OverrideDirection changed state do
			(
				if state then
					(
						DirX.enabled = DirY.enabled = DirZ.enabled = PresetX.enabled = PresetY.enabled = PresetZ.enabled = negative.enabled = ObjDir.enabled = PickObjDir.enabled = true
						PlacementMethod.state = 1
					) else (
						DirX.enabled = DirY.enabled = DirZ.enabled = PresetX.enabled = PresetY.enabled = PresetZ.enabled = negative.enabled = ObjDir.enabled = PickObjDir.enabled = false
					)
			)

		on PickObjDir picked obj do
			(
				cDir = -(obj.dir)
				DirX.value = cDir.x
				DirY.value = cDir.y
				DirZ.value = cDir.z
				PickObjDir.text = obj.name
			)

		on PresetX pressed do
			(
				if negative.checked then
					DirX.value = -1
				else
					DirX.value = 1
				DirY.value = DirZ.value = 0
			)

		on PresetY pressed do
			(
				if negative.checked then
					DirY.value = -1
				else
					DirY.value = 1
				DirX.value = DirZ.value = 0
			)

		on PresetZ pressed do
			(
				if negative.checked then
					DirZ.value = -1
				else
					DirZ.value = 1
				DirX.value = DirY.value = 0
			)

		on PlacementMethod changed state do
			if state != 5 then
				VertexThreshold.enabled = false
			else
				VertexThreshold.enabled = true
				
		on RotSel changed state do
			if RotSel.checked then
				ZrotHi.enabled = ZrotLo.enabled = true
			else
				ZrotHi.enabled = ZrotLo.enabled = false

		on RndTilt changed state do
		 if state then tilt.enabled = true else tilt.enabled = false
		
		on RndScale changed state do
			if state then (rndScaleLo.enabled = rndScaleHi.enabled = true) else (rndScaleLo.enabled = rndScaleHi.enabled = false)
		
		on rndScaleLo changed state do
			if rndScaleLo.value > rndScaleHi.value do rndScaleHi.value = rndScaleLo.value
		
		on rndScaleHi changed state do
			if rndScaleHi.value < rndScaleLo.value do rndScaleLo.value = rndScaleHi.value
		
		on Pick_Terrain picked obj do
			(
				Terrain_surface = obj
				Pick_Terrain.text = Terrain_surface.name
				Pick_PlaceSurface.text = ""
			)

		on Pick_PlaceSurface picked obj do
			(
				Terrain_surface = obj
				Pick_PlaceSurface.text = Terrain_surface.name
				Pick_Terrain.text = ""
			)
		
		-- ********************************************************************************
		-- * Place Many 
		-- ********************************************************************************
		on place_many pressed do
			(
				undo on
					(
						-- Make sure the terrain object hasn't been deleted
						terrain_surface = undefined
						for o in geometry do if (o.name == Pick_Terrain.text) or (o.name == Pick_PlaceSurface.text) do terrain_surface = o
						if terrain_surface != undefined then
							(
								VertThresh = VertexThreshold.value
								taperay = ray [0,0,0] (normalize [DirX.value, DirY.value, DirZ.value])
								TapeHeight = ((terrain_surface.max).z + 1.0)
								Current_Obj_count = 0
								Sel_obj_count = 0
								SelObjects = #()
								ProgressStart "Distributing Trees"
								if SmartMode.checked then
									for b in selection do
										if (substring b.name (b.name.count - 2) 3) != "-00" do
											append SelObjects b
								else
									SelObjects = selection as array

								Sel_obj_count = SelObjects.count
								for i = 1 to Sel_obj_count do
									(
										b = SelObjects[i]
										if debug do format "%\n" b.name
										if b.parent == undefined do	
											(
												Current_Obj_count = (Current_Obj_count + 1);
												ProgressValue = (((Current_Obj_Count as float) / (Sel_obj_count as float)) * 100);
												ProgressUpdate (ProgressValue);
									
												-- random scale
												if PlacementMethod.state != 3 do
													if RndScale.checked == true do
														(
															ScaleFactor = ((random RndScaleLo.value RndScaleHi.value) * 0.01)
															b.scale = [ScaleFactor, ScaleFactor, ScaleFactor]
														)
									
												-- random tilt
												if RndTilt.checked == true do
													(
														rotate b (random tilt.value (tilt.value * -1)) x_axis
														rotate b (random tilt.value (tilt.value * -1)) y_axis
													)
							
												-- random Z rotation
												if PlacementMethod.state != 3 do
													if RotSel.checked == true do 
														(
															Hi = ZRotHi.value
															Lo = ZRotLo.value
															bpos = b.pos
															-- rotate b (random Lo Hi) z_axis
															b.rotation = (quat 0.0 0.0 1.0 (random Lo Hi))
															b.pos = bpos
														)
							
												-- position object
												if OverrideDirection.checked then
													Taperay.pos = b.pos
												else
													Taperay.pos = [(b.pos).x, (b.pos).y, TapeHeight]

												if (ir = intersectray terrain_surface taperay) != undefined do
													(
														if classof b == Editable_Mesh do
															(
																-- conform selected verts
																if conformSelected.checked then
																(
																	ObjHeightOffset = ((b.pivot).z - (b.min).z)
																	if ConformOnly.checked == false do
																		b.pos = [(ir.pos).x, (ir.pos).y, ((ir.pos).z + HeightOffset.value + ObjHeightOffset)]
																	CVertSel = getvertselection b
																	if CVertSel.count != 0 do
																	(
																		for j = 1 to CVertSel.count do
																		(
																			CVertPos = getvert b CVertSel[j]
																			Taperay.pos = [CVertPos.x, CVertPos.y, CVertPos.z + 100]
																			if (ir = intersectray terrain_surface taperay) != undefined do
																				setvert b CVertSel[j] ir.pos

																			if ApplyRenaming.checked do
																				if (substring b.name (b.name.count - 2) 3) != "-00" do
																					b.name = "rename me"
																		)
																	update b
																	)
																) else (
																	if PlacementMethod.state == 1 do
																		(
																			ObjHeightOffset = ((b.pivot).z - (b.min).z)
																			b.pos = [(ir.pos).x, (ir.pos).y, ((ir.pos).z + HeightOffset.value + ObjHeightOffset)]
																		)
																	-- VertexPlacement method
																	if PlacementMethod.state == 2 do
																		(
																			LowVertPos = (getvert b (LowestVert b))
																			LowVertOffset = (LowVertPos - b.pivot)
																			Taperay.pos = [LowVertPos.x, LowVertPos.y, TapeHeight]
																			ir = intersectray terrain_surface taperay
																			ObjHeightOffset =((b.pivot).z - LowVertPos.z)
																			b.pos = [(ir.pos).x - LowVertOffset.x, (ir.pos).y - LowVertOffset.y, ((ir.pos).z + HeightOffset.value + ObjHeightOffset)]
																		)
																	-- Normal Placement method
																	if PlacementMethod.state == 3 do
																		(
																			Taperay.pos = [b.pos.x, b.pos.y, (terrain_surface.max.z + 100)]
																			ir = intersectray terrain_surface taperay
																			if ir != undefined do
																				(
																					bscale = b.scale
																					b.dir = [0,0,1]
																					ObjHeightOffset = ((b.pivot).z - (b.min).z)
																					bPos = [(ir.pos).x, (ir.pos).y, ((ir.pos).z + HeightOffset.value + ObjHeightOffset)]
																					b.dir = ir.dir
																					b.pos = bPos
																					-- set the scale back to original
																					b.scale = bscale
																					if RotSel.checked == true do 
																						(
																							Hi = ZRotHi.value
																							Lo = ZRotLo.value
																							in coordsys local rotate b (random Lo Hi) z_axis
																						)
																				)
																		)
																	-- Boulder placement method
																	if PlacementMethod.state == 4 do
																		(
																			Taperay.pos = [b.pos.x, b.pos.y, (terrain_surface.max.z + 100)]
																			ir = intersectray terrain_surface taperay
																			if ir != undefined do
																				(
																					-- grab the scale
																					bscale = b.scale
																					b.dir = [0,0,1]
																					ObjHeightOffset = ((b.pivot).z - (b.min).z)
																					bPos = [(ir.pos).x, (ir.pos).y, ((ir.pos).z + HeightOffset.value + ObjHeightOffset)]
																					b.dir = ir.dir
																					-- set the scale back to original
																					b.scale = bscale
																					if RotSel.checked == true do 
																						(
																							Hi = ZRotHi.value
																							Lo = ZRotLo.value
																							in coordsys local rotate b (random Lo Hi) z_axis
																						)
																					-- place based on lowest vertex....
																					LowVertPos = (getvert b (LowestVert b))
																					LowVertOffset = (LowVertPos - b.pivot)
																					Taperay.pos = [LowVertPos.x, LowVertPos.y, TapeHeight]
																					ir = intersectray terrain_surface taperay
																					ObjHeightOffset =((b.pivot).z - LowVertPos.z)
																					b.pos = [(ir.pos).x - LowVertOffset.x, (ir.pos).y - LowVertOffset.y, ((ir.pos).z + HeightOffset.value + ObjHeightOffset)]
																				)
																		)

																	-- Planted placement method
																	if PlacementMethod.state == 5 do
																		(
																			b.pos.z += 1000.0
																			Taperay.pos = [b.pos.x, b.pos.y, (terrain_surface.max.z + 100)]
																			ir = intersectray terrain_surface taperay
																			if ir != undefined do
																				(
																					ObjHeightOffset = ((b.pivot).z - (b.min).z)

																					-- place based on lowest vertex that is highest off terrain
																					LowVertArray = LowestVerts b VertThresh
																					smDistance = 0
																					LowVertPos = [0,0,0]
																					Cir = ray [0,0,0] [0,0,0]
																					continue = false
																					for j = 1 to LowVertArray.count do
																						(
																							CIndex = LowVertArray[j]
																							CVert = getVert b CIndex
																							tapeRay.pos = [CVert.x, CVert.y, TapeHeight]
																							if (ir = intersectray terrain_surface taperay) != undefined do
																								(
																									irPos = ir.pos
																									if (CDist = (CVert.z - irPos.z)) > smDistance do
																										(
																											continue = true
																											smDistance = CDist
																											LowVertPos = CVert
																											Cir = ir
																										)
																								)
																						)
																					if continue do
																						(
																							LowVertOffset = (LowVertPos - b.pivot)
																							Taperay.pos = [LowVertPos.x, LowVertPos.y, TapeHeight]
																							ObjHeightOffset =((b.pivot).z - LowVertPos.z)
																							b.pos = [(Cir.pos).x - LowVertOffset.x, (Cir.pos).y - LowVertOffset.y, ((Cir.pos).z + HeightOffset.value + ObjHeightOffset)]
																						)
																				)
																		)
																)
													)
												-- Selected is a group
												if classof b == Dummy do
													(
														ObjHeightOffset = (b.pivot.z - b.min.z)
														b.pos = [(ir.pos).x, (ir.pos).y, ((ir.pos).z + HeightOffset.value + ObjHeightOffset)]
													)
									
												if classof b == targetcamera do (b.pos = [(ir.pos).x, (ir.pos).y, ((ir.pos).z + HeightOffset.value)])
												if classof b == freecamera do (b.pos = [(ir.pos).x, (ir.pos).y, ((ir.pos).z + HeightOffset.value)])
											)
										if getProgressCancel == true then exit;
									)
								)
								ProgressEnd();
								results.text = (Sel_obj_count as string + " objects placed!")
							) else (
								messageBox "The terrain object is either undefined or has been deleted."
								Pick_Terrain.text = ""
							)
					)
			)
	)
