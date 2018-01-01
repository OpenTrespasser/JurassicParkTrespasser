

fn HardReset2 obj = 
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

obj.scale = o_scale
obj.rotation = o_rot
obj.pos = o_pos
return obj
)


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