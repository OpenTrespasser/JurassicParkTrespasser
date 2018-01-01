filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HardReset.ms"

fn DetachSelectedFaces m =
(
	ofs = getFaceSelection m					-- original selected faces
	ofvs = for f in ofs collect getFace m f  	-- original face verts
	-- build an old-to-new vertex map
	vmap = #()
	i = 0
	for f in ofvs do
		(
			if vmap[f.x] == undefined do vmap[f.x] = (i += 1)
			if vmap[f.y] == undefined do vmap[f.y] = (i += 1)
			if vmap[f.z] == undefined do vmap[f.z] = (i += 1)
		)
	-- build the new vertex coord array
	nv = #()
	for i in 1 to vmap.count do
		if vmap[i] != undefined do nv[vmap[i]] = getVert m i
	-- build the new face array
	nf = for f in ofvs collect [vmap[f.x], vmap[f.y], vmap[f.z]]
	moo = mesh vertices:nv faces:nf
	addmodifier moo (UVWMap())
	convertToMesh moo
	return moo
)

fn TotalReset obj = 
(
gc ()

local CObj = obj
ConvertToMesh CObj
CObj.pivot = CObj.center
Props = GetUserPropBuffer CObj

-- Copy the object.  Center the pivot.
--local o_pos = obj.pos
--local o_rot = obj.rotation
--local o_scale = obj.scale

-- move the object to the origin, strip off rotation, scale
--obj.pos = [0,0,0]
--obj.rotation = (quat 0 z_axis)
--obj.scale = [1,1,1]

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

-- do not fix location of new object- leave it zeroed.

--m.scale = o_scale
--m.rotation = o_rot
--m.pos = o_pos

-- other stuff

m.wirecolor = CObj.wirecolor


update m
if CObj.mat != undefined do
	m.mat = CObj.mat

local oname = CObj.name
delete CObj
m.name = oname
setUserPropBuffer m Props

return m;
)

fn Volume obj =
(
	local v3Min = GetVert obj 1
	local v3Max = GetVert obj 1
	local MyVert = v3Max
	
-- returns the volume of the axis aligned bounding box around obj
	for i = 1 to obj.numverts do
	(
		MyVert = GetVert obj i
		if (v3Min.x > MyVert.x) do
			v3Min.x = MyVert.x
		if (v3Min.y > MyVert.y) do
			v3Min.y = MyVert.y
		if (v3Min.z > MyVert.z) do
			v3Min.z = MyVert.z
		
		if (v3Max.x < MyVert.x) do
			v3Max.x = MyVert.x
		if (v3Max.y < MyVert.y) do
			v3Max.y = MyVert.y
		if (v3Max.z < MyVert.z) do
			v3Max.z = MyVert.z
	)

--	extents = obj.max - obj.min;
	local extents = v3Max - v3Min
	return extents.x * extents.y * extents.z;
)

fn Random11 = 
(
	return Random -1.0 1.0;
)

fn RandomQuaternion = 
(
	return (quat (random 0 180) [Random11(),Random11(),Random11()]);
)

fn RandomRotate obj = 
(
-- Randomly rotates obj, and returns the rotation.
	local quaternion = randomQuaternion();
	obj.rotation = quaternion;
	return quaternion;
)

fn BoxMe obj count = 
(
	convertToMesh obj

	local rot = obj.rotation;
	local vsmall = volume obj;
	local qsmall = rot;

	-- temp variables
	local q = rot;
	local v = vsmall;

	for i = 1 to (count as integer) do
	(
		q = RandomRotate obj;
		v = volume obj;
		if v < vsmall do
		(
			vsmall = v;
			qsmall = q;
		)
	)
	
	delta = rot - qsmall;
	obj.rotation = qsmall;
	--	obj.rotation = qsmall + delta;
	
	local foo = TotalReset obj;
	foo.rotation = delta;
	return delta
)

-- ***********************************
-- * Start Utility
-- ***********************************
Utility BoundBox "Bounding Box"
(
	group "Options"
		(
			spinner NumTries "Num tries: " range:[1,99999,1000] type:#integer
			checkbox genBox "Generate Box Geometry"
			checkbox fromFaces "From Face Selection"
			PickButton GetObject "Pick Object" width:120
		)

	group "Local space Boxes"
		(
			pickButton BoxFromBBox "Box from BBox" width:120
			button BoxFromBBoxSel	 "Boxes from Selection" width:120
		)

	on BoxFromBBox picked obj do
		(
			oRot = obj.rotation
			oPos = obj.pos
			oOffset = obj.pivot - obj.center
			obj.rotation = (quat 0 0 0 1)
			obj.pos = oPos
			b = box height:((obj.max).z - (obj.min).z) width:((obj.max).x - (obj.min).x) length:((obj.max).y - (obj.min).y)
			b.pivot = b.center
			b.pos = obj.center
			b.pivot = obj.pivot
			b.rotation = oRot
			b.pos = oPos
			b.name = obj.name
			obj.rotation = oRot
			obj.pos = oPos
		)

	on BoxFromBBoxSel pressed do
		(
			ObjArray = selection as array
			oc = ObjArray.count
			for i = 1 to oc do
				(
					obj = ObjArray[i]
					oRot = obj.rotation
					oPos = obj.pos
					oOffset = obj.pivot - obj.center
					obj.rotation = (quat 0 0 0 1)
					obj.pos = oPos
					b = box height:((obj.max).z - (obj.min).z) width:((obj.max).x - (obj.min).x) length:((obj.max).y - (obj.min).y)
					b.pivot = b.center
					b.pos = obj.center
					b.pivot = obj.pivot
					b.rotation = oRot
					b.pos = oPos
					b.name = ("F" + obj.name)
					obj.rotation = oRot
					obj.pos = oPos
				)
		)

	
	on GetObject picked obj do
		(
			if fromFaces.checked then
				(
					CObj = (DetachSelectedFaces obj)
					CObj.name = (obj.name + "_submesh")
				) else (
					CObj = obj
				)
			CObj = TotalReset CObj
			TName = CObj.name
			BoxRot = BoxMe CObj NumTries.value
			if genBox.checked do
				(
					for o in objects do
						if o.name == TName do CObj = o
					TObj = copy CObj
					-- give the object no rotation (identity quat)
					TObj.rotation = (quat 0 [0,0, 0])
					TObj = TotalReset TObj
					v3Min = GetVert Tobj 1
					v3Max = GetVert Tobj 1
					for i = 1 to TObj.numverts do
						(
							MyVert = GetVert TObj i
							if (v3Min.x > MyVert.x) do v3Min.x = MyVert.x
							if (v3Min.y > MyVert.y) do v3Min.y = MyVert.y
							if (v3Min.z > MyVert.z) do v3Min.z = MyVert.z
							if (v3Max.x < MyVert.x) do v3Max.x = MyVert.x
							if (v3Max.y < MyVert.y) do v3Max.y = MyVert.y
							if (v3Max.z < MyVert.z) do v3Max.z = MyVert.z
						)
					format "Max:%\nMin%\n" v3Max v3Min
					b = box height:(v3Max.z - v3Min.z) width:(v3Max.x - v3Min.x) length:(v3Max.y - v3Min.y) mapcoords:true
					b.pivot = b.center
					b = (hardReset b)
					b.pos = TObj.center
					delete TObj
					b.rotation = BoxRot
					b.name = (TName + "_Bbox")
					if fromFaces.checked do delete CObj
				)

		)
)

