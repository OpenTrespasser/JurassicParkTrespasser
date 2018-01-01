fn HardReset obj = 
(
gc ()

local CObj = obj
ConvertToMesh CObj
CObj.pivot = CObj.center
Props = GetUserPropBuffer CObj

-- Copy the object.  Center the pivot.
local o_pos = obj.pos
local o_rot = obj.rotation
local o_scale = obj.scale

-- move the object to the origin, strip off rotation, scale
obj.pos = [0,0,0]
obj.rotation = (quat 0 z_axis)
obj.scale = [1,1,1]

-------

local VertArray = for i = 1 to CObj.numverts collect (getvert CObj i)
local FaceArray = for i = 1 to CObj.numfaces collect (getface CObj i)
local MatIDArray = for i = 1 to CObj.numfaces collect (GetFaceMatID CObj i)
local TVertArray = #()

local i = 0
for i = 1 to FaceArray.count do
	(
		CFace = (GetTVFace CObj i)
		if FindItem TVertArray CFace.x == 0 do (append TVertArray CFace.x)
		if FindItem TVertArray CFace.y == 0 do (append TVertArray CFace.y)
		if FindItem TVertArray CFace.z == 0 do (append TVertArray CFace.z)
	)
TVertUVArray = (for i = 1 to TVertArray.count collect (GetTvert CObj TVertArray[i]))
local m = mesh vertices:VertArray faces:FaceArray MaterialIDs:MatIDArray
for i = 1 to CObj.numfaces do
	(
		SetEdgeVis m i 1 (GetEdgeVis CObj i 1)
		SetEdgeVis m i 2 (GetEdgeVis CObj i 2)
		SetEdgeVis m i 3 (GetEdgeVis CObj i 3)
	)
for i = 1 to CObj.numfaces do
	SetFaceSmoothGroup m i (getFaceSmoothGroup CObj i)
addmodifier m (uvwMap maptype:1)
ConvertToMesh m

local Ntv = getNumTVerts CObj
SetNumTverts m Ntv true

for i = 1 to CObj.numfaces do
	setTVFace m i (GetTVFace CObj i)

update m
for i = 1 to Ntv do
	SetTvert m i (GetTvert CObj i)
m.name = (CObj.name + "_regen")
m.pivot = m.center

-- fix location of new object

m.scale = o_scale
m.rotation = o_rot
m.pos = o_pos

-- other stuff

m.wirecolor = CObj.wirecolor


update m
if CObj.mat != undefined do
	m.mat = CObj.mat

local oname = CObj.name
delete CObj
m.name = oname
setUserPropBuffer m Props
return m
)

fn GetGUIAppScale obj =
(
	local rot = obj.rotation;
	obj.rotation = (quat 0 Z_axis);
	
	local dim = obj.max - obj.min;
	local GUIscale = dim.x;
	if (dim.y > GUIScale) do
		GUIscale = dim.y;
	if (dim.z > GUIScale) do
		GUIscale = dim.z;
	
	obj.rotation = rot;
	
	return GUIscale * 0.5;
)

fn UpdateObject obj word_array = 
(
	local len, angle, x, y, z, w;
	len = word_Array.count;
--	print word_array;

	-- Read from right to left.
	-- Scale is the last number
	local rescale = GetGUIAppScale obj;
	rescale = word_array[len] as float / rescale;
-- 	obj.scale = obj.scale * rescale;
	-- ignore scale
	len = len - 1;
		
	-- Rotation is the next four
	w = word_array[len-3] as float;
	x = word_array[len-2] as float;
	y = word_array[len-1] as float;
	z = word_array[len-0] as float;
	obj.rotation = (quat x y z w);
	len = len - 4;

	-- Position is the next three.
	x = word_array[len-2] as float;
	y = word_array[len-1] as float;
	z = word_array[len-0] as float;
	obj.pos = [x, y, z]
	len = len - 3;
)

fn WordArray textline = 
(
	local c;
	local i = 1;
	local word = "";
	local word_array = #();
	
	for i = 1 to textline.count do
		(
			c = textline[i];
			if (c == " " or c == "\t" or c == "\n") then
				(
					append word_array word
					word = "";
				)
			else
				word = word + c;
		)
	append word_array word;
)

fn load_placement_file filename = 
	(
		-- loads the file into an array of arrays.  Each array in the 
		--	containing array is a line of text as a WordArray.
		local FileArray = #();
		
		local f, l;
		f = openfile filename; 
		if f != undefined then
			(
				while not eof f do
					(
						l = readline f;
						append FileArray (WordArray l);
					)
				close f;	
			)
		else
			format "File not found: %\n" filename

		return filearray
	)

fn find_array_index obj filearray = 
		(
			-- object name should not have spaces!
			if (wordArray obj.name).count != 1 then
				format "Object name cannot have spaces... => %\n" obj.name
			else
				(
					-- Find object in array.
					arraylength = filearray.count;
					
					for i = 1 to arraylength do
						(
							linelength = filearray[i].count;
							if (obj.name == filearray[i][linelength - 8]) do
								(
									-- found it!  Now update it.
									return i;
								)
						)
				)
			return undefined;
		)

fn update_obj_from_array obj filearray =
	(
		local i;
		i = find_array_index obj filearray;
	
		if i != undefined then
			(
				updateObject obj filearray[i]
				return true
			)
		else
			(
				format "Object not found: %\n" obj.name
				return false
			)
	)

-- **************************************************************************************************
-- * Utility Start
-- **************************************************************************************************
Utility LoadGroffText "Load Groff Text"
	(
		checkbox HR "Hard Reset" checked:true
		button GetGroffTextFile "Load Groff Text Selection" width:145
		on GetGroffTextFile pressed do
			if (GroffTextFile = getOpenFilename caption:"Select Groff Text File:") != undefined do
			(
				g = geometry as array
				CFileArray = load_placement_file GroffTextFile
				if CFileArray != undefined then
				(
					-- build the valid object array
					ObjArray = for o in selection collect o
					for i = 1 to ObjArray.count do
					(
						CObj = ObjArray[i]
						if (findItem g CObj) != 0 do
							if (update_obj_from_array CObj CFileArray) == true do
								if HR.checked do
									CObj = HardReset CObj
					)
				)
				else
					format "CFileArry is undefined...aborting\n"							
			)
	)