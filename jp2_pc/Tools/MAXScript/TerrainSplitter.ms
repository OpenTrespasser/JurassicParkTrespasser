--**********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of TerrainSplitter.ms
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
fn detach_selected_faces m count =
(
	ofs = getFaceSelection m
	ofvs = for f in ofs collect getFace m f
	vmap = #()
	i = 0
	for f in ofvs do
	(if vmap[f.x] == undefined do vmap[f.x] = (i += 1)
	if vmap[f.y] == undefined do vmap[f.y] = (i += 1)
	if vmap[f.z] == undefined do vmap[f.z] = (i += 1))
	nv = #()
	for i in 1 to vmap.count do
	if vmap[i] != undefined do nv[vmap[i]] = getVert m i
	nf = for f in ofvs collect [vmap[f.x], vmap[f.y], vmap[f.z]]
   mesh name:("TerrainSection" + (count as string)) vertices:nv faces:nf 
)

fn resettransform obj = 
(
local otm = obj.transform
obj.transform = matrix3 1
local xf = xform()
addmodifier obj xf
xf.gizmo.transform = otm
)

Utility TerrainSplitter "Terrain Splitter"
(
local TerrainObj, TerrainObjDimX, TerrainObjDimY, TerrainObjDimZ, SplitResolution, NumBoxesX, 
NumBoxesY, NewBoxName, boxcount = 1, NewPosition, OriginOffsetx, OriginOffsety, newx, newy, 
CurrentBox, GizmoOffset, TerrainObjVol, OffsetY, OffsetX, facearray, vertarray, SelVertarray,
m, CurrentFaceSel, Vertarrayall, taperay, SectionBox, TerrainOriginOffsetx, TerrainOriginOffsety, 
numstepsx, numstepsy, ProgScale, TerrainSections, BoundingBoxes, ZPos, DataPointCount;

label version "v1.0"
group "Options"
(
Label dlbtns1 "Split Detail Level:" align:#Left
DropDownlist DetailLvl_btns Items: #("4 Meters", "8 Meters","16 Meters", "32  Meters", "64  Meters", "128  Meters") Selection:2
label ExportDetail "Export Detail Level:" align:#left
DropDownList ExportDetailLvl_btns items: #("1/4 Meter", "1/2 Meter", "1 Meter", "2  Meters", "4  Meters", "8  Meters", "16  Meters", "32  Meters") Selection:6
checkbox exportchk "Export Terrain?" checked:true
)
pickbutton PickSplitObject_btn "Choose Terrain"
label blank ""
group "Export Progress"
(
label prog1 ""
label prog2 ""
)

on PickSplitObject_btn picked obj do
	(
		Taperay = ray [0,0,0] [0,0,-1]
		format "Starttime: %\n" localtime
		terrainobj = obj
		if exportchk.checked == true do (save_filename = getsavefilename caption:"Specify Data Dump File");
		OriginOffsetx = (TerrainObj.min).x
		OriginOffsety = (TerrainObj.min).y
		TerrainObjDimX = ((TerrainObj.max).x - (TerrainObj.min).x)
		TerrainObjDimY = ((TerrainObj.max).y - (TerrainObj.min).y)
		TerrainObjDimZ = ((TerrainObj.max).z - (TerrainObj.min).z)
		if DetailLvl_btns.selection == 1 do (SplitResolution = 4)
		if DetailLvl_btns.selection == 2 do (SplitResolution = 8)
		if DetailLvl_btns.selection == 3 do (SplitResolution = 16)
		if DetailLvl_btns.selection == 4 do (SplitResolution = 32)
		if DetailLvl_btns.selection == 5 do (SplitResolution = 64)
		if DetailLvl_btns.selection == 6 do (SplitResolution = 128)
		NumBoxesX = (TerrainObjDimX / SplitResolution)
		NumBoxesY = (TerrainObjDimY / SplitResolution)
		BoxX = (TerrainObjDimX / NumBoxesX)
		BoxY = (TerrainObjDimY / NumBoxesY)
		NumBoxes = (NumBoxesX * NumBoxesY)
		newy = (OriginOffsety + (SplitResolution / 2))
		ProgressStart "Splitting up Mesh"
		Boxcountz = 0
		for j = 1 to NumBoxesY do
			(
			Newx = (OriginOffsetx + (SplitResolution / 2))
			for i = 1 to NumBoxesX do
			(
		Boxcountz = Boxcountz +1
		prog1.text = ("Splitting " + Boxcountz as string + " of " + NumBoxes as string)
		if ProgressUpdate ((Boxcountz/NumBoxes) *100) == false then exit
		CurrentBox = box name: ("BoundingBox" + (Boxcountz as string)) Width:BoxX  Length:BoxY Height:(2*(TerrainObjDimZ)) pos:[newx,newy,0]
--reset transform on Current Box
			o_pos =CurrentBox.pos
			CurrentBox.pos = [0,0,0]
			resettransform CurrentBox
			CurrentBox.pos = o_pos
			ConvertToMesh CurrentBox
-- start volume selection
			addmodifier CurrentBox (volumeSelect())
			addmodifier terrainobj (volumeSelect())
			TerrainObj.modifiers[1].level = 2
			TerrainObj.modifiers[1].type = 1
			TerrainObjVol = TerrainObj.modifiers[1]
			BoxObjVol = CurrentBox.modifiers[1]
			BoxObjVol.gizmo.pos = [0,0,0]
			CurrentBox.pivot = TerrainObj.pivot
			PivotOffset = (CurrentBox.pivot - CurrentBox.center)
			TerrainObjVol.gizmo.scale = BoxObjVol.gizmo.scale
			GizmoOffset = (TerrainObj.pos - CurrentBox.pos)
			TerrainObjVol.gizmo.pos = TerrainObj.ObjectOffsetPos * -1
			TerrainObjVol.gizmo.pos = -(PivotOffset)
-- detach Selected Faces
			ConvertToMesh TerrainObj
			update TerrainObj	
			detach_selected_faces TerrainObj Boxcountz
			newx = (newx + SplitResolution)
			setfaceselection TerrainObj #()
			update TerrainObj
		)
	newy = newy + SplitResolution
)
ProgressEnd()
prog1.text = "Splitting Done..."

if exportchk.checked do
(
TerrainSections = for obj in $TerrainSection* collect obj
for i in 1 to TerrainSections.count do (TerrainSections[i].pivot = TerrainSections[i].center)
BoundingBoxes = for obj in $BoundingBox* collect obj

if ExportDetailLvl_btns.Selection == 1 do (resolution = 0.25)
if ExportDetailLvl_btns.selection == 2 do (resolution = 0.5)
if ExportDetailLvl_btns.selection == 3 do (resolution = 1)
if ExportDetailLvl_btns.selection == 4 do (resolution = 2)
if ExportDetailLvl_btns.selection == 5 do (resolution = 4)
if ExportDetailLvl_btns.selection == 6 do (resolution = 8)
if ExportDetailLvl_btns.selection == 7 do (resolution = 16)
if ExportDetailLvl_btns.selection == 8 do (resolution = 32)

numstepsx = (BoxX/resolution) as integer
numstepsy = (BoxY/resolution) as integer

ProgressStart "Exporting. . . "
f = createfile save_filename
	format "%\n" (terrainobj.max.x - terrainobj.min.x) to:f
	format "%\n" (terrainobj.max.y - terrainobj.min.y) to:f
	format "%\n" (terrainobj.min).x to:f
	format "%\n" (terrainobj.min).y to:f
	format "%\n" resolution to:f
	format "%\n" ((((TerrainObjDimX / resolution)+ NumBoxesX) * (((TerrainObjDimY / resolution))+ NumBoxesY) as integer)) to:f
DataPointCount = 0

TSc = TerrainSections.count
for i = 1 to TSc do
		(
			prog2.text = ("Section # " + i as string + " of " + TSc as string)
			SectionObject = Terrainsections[i]
			taperay.pos = [(SectionObject.pos).x, (SectionObject.pos).y, 10000]
			for j = 1 to BoundingBoxes.count do (if (intersectray BoundingBoxes[j] taperay) != undefined do (SectionBox = BoundingBoxes[j]))
			TerrainOriginOffsetx = (SectionBox.min).x
			TerrainOriginOffsety = (SectionBox.min).y
			newx = TerrainOriginOffsetx
			newy = TerrainOriginOffsety
			progscale = (100.0 /numstepsx)
			for x = 0 to numstepsx do
				(
					if ProgressUpdate (x * progscale) == false then exit
					newx = TerrainOriginOffsetx + (x * resolution)
					for y = 0 to numstepsy do
						(
						newy = TerrainOriginOffsety + (y * resolution)
						taperay.pos = [(newx as float), (newy as float),10000.0]
						format "% % " newx newy to:f
						lc = 0
						while (intersectray SectionObject TapeRay) == undefined do 
							(
								lc = lc + 1
								taperay.pos = [(newx as float), (newy as float),10000.0]
								taperay.pos += [(random -0.1 0.1 ),(random -0.1 0.1 ), 10000.0]
								if ProgressUpdate == false then exit
								if lc > 10 do (lc = -1)
							)
						if lc == -1 then 
						(
							ZPos = 1000.0
							) else (
							ZPos = (intersectray SectionObject TapeRay)
						)
						format "%\n" ((Zpos.pos).z) to:f
						DataPointCount = (DataPointCount +1)
						)
				)
		)
ProgressEnd()
close f;
format "DataPointCount = %\n" DataPointCount
prog2.text = "Exporting Done!"
)
format "EndTime: %\n" localtime
))



