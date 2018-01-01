--**********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *					Implementation of VPlace.ms 
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *					VPlace places selected vertices of an object at a certain height 
-- * 					this height is eithere determined to be the top or bottom limit of a 
-- * 					picked object, or it is determined from user input via a spinner control.
-- *					Road Support Added too (6/6).
-- **********************************************************************************************

Utility VPlace "V-Place with Roads"
(
local SnapObject, NewHeight, SnapTop, SnapBottom, tapeobj, terrainobj, road, loftspline;

group "V-Place Basic Options"
	(
		Spinner SpinHeight "" range:[0,1000,0] type:#float fieldwidth:50 offset:[3,0]
		checkbutton UseSpinHeight "Lock Height:" offset:[-31,-22]
		pickbutton GetSnapObject "Pick Snap Object" width:140
		label snapOptionslabel "Snap Object Options:" align:#left
		radiobuttons HeightKeys labels:#("Top of Mesh","Bottom of Mesh", "Center of Mesh", "Mesh Pivot Point", "Avg Vert Selection Height") default:2 align:#left
		label SnapObjLabel "" align:#left
		label ht "" align:#left
		label vp "" align:#left
		pickbutton PickObject "Pick Terrain" width:135
	)

group "V-Place Conform Tools"
	(
		label splineLabel "Conform this Spline:" align:#left
		pickbutton ChooseSpline "-" width:105 offset:[-17,0]
		Spinner SplineResolution "" range:[0,1,0] type:#integer fieldwidth:25 offset:[4,-24]
		label terrainlabel "Conform this Mesh:" align:#left
		pickbutton ChooseTerrain "-" width:110 offset:[-15,0]
		checkbutton conformSelected "Sel" offset:[57,-26] 
		label roadlabel "To this Mesh:" align:#left
		pickbutton chooseRoad "-" width:140
		label blank ""
		button ConformSpline "Conform Spline" width:140
		button pave "Conform Mesh" width:140
	)

on UseSpinHeight changed state do
	if state == on then
		(
			GetSnapObject.enabled = false
			NewHeight = SpinHeight.value
			ht.text = ("Height set at: " + NewHeight as string + "M")
			SnapObjLabel.text = ("Snap Object: none")
			SpinHeight.enabled = false
			HeightKeys.enabled = false
		) else (
			GetSnapObject.enabled = true
			SpinHeight.enabled = true
			HeightKeys.enabled = true
		)

on SpinHeight changed state do
	(
		NewHeight = SpinHeight.value
		ht.text = ("Height set at: " + NewHeight as string + "M")
		SnapObjLabel.text = ("Snap Object: none")
	)
		

on ChooseSpline picked obj do
(
loftSpline = obj
ConvertToSplineShape loftspline
if classof loftspline != splineshape then 
	(
		messageBox "You must pick a collapsed spline object"
		loftSpline = undefined
		ChooseSpline.text = "Choose Spline"
	) else (
		ChooseSpline.text = loftSpline.name
	)
)
		
on GetSnapObject picked Obj do
	(
		SnapObject = obj
		SnapObjLabel.text = ("Snap Object: " + SnapObject.name)
		SnapTop = (SnapObject.max).z
		SnapBottom = (SnapObject.min).z
		SnapCenter = (SnapObject.center).z
		SnapPivot = (SnapObject.pivot).z
		if HeightKeys.state == 1 do (NewHeight = SnapTop)
		if HeightKeys.state == 2 do (NewHeight = SnapBottom)
		if HeightKeys.state == 3 do (NewHeight = SnapCenter)
		if HeightKeys.state == 4 do (NewHeight = SnapPivot)
		ht.text = ("Height set at: " + NewHeight as string + "M")
		vp.text = ""
	)

on PickObject picked obj do
	(
		undo on 
			(
				Terrain = obj
				ConvertToMesh Terrain
				VertSel = (getvertselection Terrain)
				vc = VertSel.count
				if vc == 0 do (MessageBox "No Vertices Scelected")
				if UseSpinHeight.checked == true do 
					(
						NewHeight = SpinHeight.value
						SnapObjLabel.text = ("Snap Object: ignored")
					)
				if UseSpinHeight.checked == false do (if NewHeight == undefined do (NewHeight = -1.0))
				ht.text = "Height set at: " + NewHeight as string
				if HeightKeys.state == 5 do
					(
						TotalHeight = 0.0
						for i = 1 to vc do
							TotalHeight = (getvert Terrain VertSel[i]).z + TotalHeight
						NewHeight = TotalHeight / (vc as float)
					)
				for i = 1 to vc do
					(
						VertIndex = VertSel[i]
						VertPos = getvert terrain VertIndex
						SetVert Terrain VertIndex [VertPos.x, VertPos.y, NewHeight]
					)
				Update Terrain
				vp.text = (vc as string + " Vertices Translated")
			)
	)


on getTape picked obj do 
	(
		tapeobj = obj
		getTape.text = tapeobj.name	
	)
		
on pave pressed do
(
	if terrainobj == undefined do 
		(
			messageBox "Error!!! No Terrain Defined.  Exiting..."
			Exit
		)
	if road == undefined do 
		(
			messageBox "Error!!! No Road Defined.  Exiting..."
			Exit
		)

	if ConformSelected.checked == true do
		(
			ProgressStart "Paving..."
			heightOffset = ((road.max).z + 1)
			VertSelArray = getvertselection terrainobj
			nv = VertSelArray.count
			if nv == 0 do (messagebox "No Vertices Selected!"; exit)
			progscale = 100.0 / nv
			r = ray [0,0,0] [0,0,-1]
			for i = 1 to nv do
				(
					vpos = getvert TerrainObj VertSelArray[i]
					r.pos = [vpos.x, vpos.y, heightOffset]
					if (ir = intersectRay road r) != undefined do
						setvert terrainObj VertSelArray[i] ir.pos
					if ProgressUpdate (i * progscale) == false then exit
				)
			ProgressEnd()
			update TerrainObj
		)

	if ConformSelected.checked == false do
		(
			ProgressStart "Paving..."
			heightOffset = ((road.max).z + 1)
			nv = terrainObj.numverts
			progscale = 100.0 / nv
			r = ray [0,0,0] [0,0,-1]
			for i = 1 to nv do
				(
					vpos = getvert TerrainObj i
					r.pos = [vpos.x, vpos.y, heightOffset]
					if (intersectRay road r) != undefined do
						setvert terrainObj i (intersectRay road r).pos
					if ProgressUpdate (i * progscale) == false then exit
				)
			ProgressEnd()
			update TerrainObj
		)
)

on ChooseTerrain picked obj do
	(
		terrainobj = obj
		ConvertToMesh terrainObj
		ChooseTerrain.text = terrainobj.name
		nv = getvertselection terrainobj
		if nv != 0 do (conformSelected.checked = true)
	)

on ChooseRoad picked obj do
	(
		road = obj
		ConvertToMesh road
		ChooseRoad.text = road.name
	)

on ConformSpline pressed do
	(
		if road == undefined do 
			(
				messageBox "Error!!! No Terrain Defined.  Exiting..."
				Exit
			)
		if loftspline == undefined do 
			(
				messageBox "Error!!! No Spline Defined.  Exiting..."
				Exit
			)
		ProgressStart "Conforming..."
		ConVertToSplineShape loftspline
		SplineRez = SplineResolution.value
		ns = loftspline.numsplines
		for i = 1 to ns do
			(
				Cnt = 0
				if SplineRez == 0 do (Exit)
				NumSegs = numsegments loftspline i
				for j = 1 to NumSegs do
					(
						for k = 1 to SplineRez do
							(
								SegmantParam = (1.0 / (SplineRez + 1.0)) * k
								refineSegment loftspline i (j + Cnt) SegmantParam
								Cnt = Cnt + 1
							)			
					)
			)
		progscale = 100.0 / ns
		r = ray [0,0,0] [0,0,-1]
		HeightOffset = (road.max).z + 1
		for i = 1 to ns do
			(
				nk = numknots loftspline i
				for j = 1 to nk do
					(
						-- SetKnotType loftspline i j #smooth
						kpos = getknotpoint loftspline i j
						r.pos = [kpos.x, kpos.y, HeightOffset]
						if (ir = intersectRay road r) != undefined then
							(
								setknotpoint loftspline i j ir.pos
							) else (
								format "Knot % returned Undefined intersection\n" j
							)
						if ProgressUpdate (i * progscale) == false then exit
					)	
			)
		ProgressEnd()
		updateshape loftspline
	)
)
