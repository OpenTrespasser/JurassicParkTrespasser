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

group "Detail Levels"
(
Label dlbtns1 "Specify Split Detail Level:" align: #Left
radiobuttons DetailLvl_btns labels: #("32  Meters", "64  Meters", "128  Meters", "256  Meters", "512  Meters", "1024  Meters") default:2
label labelblank1 ""
label ExportDetail "Specify Export Detail:" align: #left
radiobuttons ExportDetailLvl_btns labels: #("1/4 Meter", "1/2 Meter", "1 Meter", "2  Meters", "4  Meters", "8  Meters", "16  Meters", "32  Meters") default:6
)
group "Split and Export"
(
pickbutton pick_tape "Choose Tape"
label tape_name "--none--"
pickbutton PickSplitObject_btn "Split up Terrain"
label terrainobjname "-- no object --"
checkbox exportchk "Export Terrain?" checked:true
)
group "Export Progress"
(
label prog1 ""
label prog2 ""
)

on pick_tape picked obj do
(tapeobj = obj
tape_name.text = tapeobj.name
taperay = obj as ray
taperay.dir = [0,0,-1]
)

on PickSplitObject_btn picked obj do
	(
		format "Starttime: %\n" localtime
		terrainobj = obj
		terrainobjname.text = terrainobj.name
		if exportchk.checked == true do (save_filename = getsavefilename caption:"Specify Data Dump File");
		OriginOffsetx = (TerrainObj.min).x
		OriginOffsety = (TerrainObj.min).y
		TerrainObjDimX = ((TerrainObj.max).x - (TerrainObj.min).x)
		TerrainObjDimY = ((TerrainObj.max).y - (TerrainObj.min).y)
		TerrainObjDimZ = ((TerrainObj.max).z - (TerrainObj.min).z)
		if DetailLvl_btns.state == 1 do (SplitResolution = 32)
		if DetailLvl_btns.state == 2 do (SplitResolution = 64)
		if DetailLvl_btns.state == 3 do (SplitResolution = 128)
		if DetailLvl_btns.state == 4 do (SplitResolution = 256)
		if DetailLvl_btns.state == 5 do (SplitResolution = 512)
		if DetailLvl_btns.state == 6 do (SplitResolution = 1024)
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
		prog1.text = ("Box# " + Boxcountz as string + " of " + NumBoxes as string)
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

if exportchk.checked do
(
TerrainSections = for obj in $TerrainSection* collect obj
for i in 1 to TerrainSections.count do (TerrainSections[i].pivot = TerrainSections[i].center)
BoundingBoxes = for obj in $BoundingBox* collect obj

if ExportDetailLvl_btns.state == 1 do (resolution = 0.25)
if ExportDetailLvl_btns.state == 2 do (resolution = 0.5)
if ExportDetailLvl_btns.state == 3 do (resolution = 1)
if ExportDetailLvl_btns.state == 4 do (resolution = 2)
if ExportDetailLvl_btns.state == 5 do (resolution = 4)
if ExportDetailLvl_btns.state == 6 do (resolution = 8)
if ExportDetailLvl_btns.state == 7 do (resolution = 16)
if ExportDetailLvl_btns.state == 8 do (resolution = 32)

numstepsx = (BoxX/resolution) as integer
numstepsy = (BoxY/resolution) as integer

ProgressStart "Exporting. . . "
f = createfile save_filename
	format "%\n" (terrainobj.max.x - terrainobj.min.x) to:f
	format "%\n" (terrainobj.max.y - terrainobj.min.y) to:f
	format "%\n" (terrainobj.min).x to:f
	format "%\n" (terrainobj.min).y to:f
	format "%\n" resolution to:f
	format "%\n" ((((TerrainObjDimX / resolution)+2) * (((TerrainObjDimY / resolution))+2) as integer)) to:f
DataPointCount = 0

TSc = TerrainSections.count
for i = 1 to TSc do
		(
			prog1.text = "Splitting Done"
			prog2.text = ("Section " + i as string + " of " + TSc as string)
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
						while (intersectray SectionObject TapeRay) == undefined do 
							(
								taperay.pos = [(newx as float), (newy as float),10000.0]
								taperay.pos += [(random -0.1 0.1 ),(random -0.1 0.1 ), 10000.0]
								if ProgressUpdate == false then exit
							)
						ZPos = (intersectray SectionObject TapeRay);
						format "%\n" ((Zpos.pos).z) to:f
						DataPointCount = (DataPointCount +1)
						)
				)
		)
ProgressEnd()
close f;
format "DataPointCount = %\n" DataPointCount
prog2.text = "Exporting Done"
)
format "EndTime: %\n" localtime
))
